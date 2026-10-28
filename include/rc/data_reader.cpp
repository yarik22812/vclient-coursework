#include "data_reader.hpp"

#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

template <typename T>
void read_exact(std::ifstream& file, T& value, const std::string& file_name)
{
    file.read(reinterpret_cast<char*>(&value), sizeof(T));

    if (file.gcount() != static_cast<std::streamsize>(sizeof(T))) {
        throw std::runtime_error(
            "read_vectors: файл '" + file_name +
            "' повреждён или содержит неполные данные"
        );
    }
}

} // namespace

Vectors read_vectors(const std::string& file_name)
{
    std::ifstream file(file_name, std::ios::binary);

    if (!file) {
        throw std::runtime_error(
            "read_vectors: не удалось открыть файл '" + file_name + "'"
        );
    }

    file.seekg(0, std::ios::end);
    const std::streamoff file_size = file.tellg();

    if (file_size < 0) {
        throw std::runtime_error(
            "read_vectors: не удалось определить размер файла '" + file_name + "'"
        );
    }

    file.seekg(0, std::ios::beg);

    std::uint32_t vector_count = 0;
    read_exact(file, vector_count, file_name);

    // Для каждого вектора в файле должен быть хотя бы его размер.
    const std::streamoff remaining_after_count =
        file_size - static_cast<std::streamoff>(sizeof(vector_count));

    if (remaining_after_count < 0 ||
        vector_count > static_cast<std::uint64_t>(remaining_after_count) /
                           sizeof(std::uint32_t)) {
        throw std::runtime_error(
            "read_vectors: некорректное количество векторов в файле '" +
            file_name + "'"
        );
    }

    Vectors vectors;
    vectors.reserve(vector_count);

    for (std::uint32_t i = 0; i < vector_count; ++i) {
        std::uint32_t vector_size = 0;
        read_exact(file, vector_size, file_name);

        const std::streamoff position = file.tellg();
        if (position < 0 || position > file_size) {
            throw std::runtime_error(
                "read_vectors: ошибка чтения файла '" + file_name + "'"
            );
        }

        const std::streamoff remaining = file_size - position;
        const std::uint64_t available_values =
            static_cast<std::uint64_t>(remaining) / sizeof(double);

        if (vector_size > available_values) {
            throw std::runtime_error(
                "read_vectors: неполные данные вектора " +
                std::to_string(i) + " в файле '" + file_name + "'"
            );
        }

        std::vector<double> values(vector_size);

        if (!values.empty()) {
            const auto bytes = static_cast<std::streamsize>(
                values.size() * sizeof(double)
            );

            file.read(reinterpret_cast<char*>(values.data()), bytes);

            if (file.gcount() != bytes) {
                throw std::runtime_error(
                    "read_vectors: ошибка чтения значений вектора " +
                    std::to_string(i) + " из файла '" + file_name + "'"
                );
            }
        }

        vectors.push_back(std::move(values));
    }

    if (file.peek() != std::ifstream::traits_type::eof()) {
        throw std::runtime_error(
            "read_vectors: в файле '" + file_name +
            "' обнаружены лишние данные"
        );
    }

    return vectors;
}
