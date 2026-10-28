#include "data_io.hpp"

#include <fstream>
#include <limits>
#include <locale>
#include <stdexcept>
#include <string>

namespace {

template <typename T>
void read_value(std::istream& input, T& value, const std::string& filename)
{
    input.read(reinterpret_cast<char*>(&value), sizeof(T));

    if (!input || input.gcount() != static_cast<std::streamsize>(sizeof(T))) {
        throw std::runtime_error(
            "read_vectors: неожиданный конец или ошибка чтения файла '" +
            filename + "'"
        );
    }
}

} // namespace

DataVectors read_vectors(const std::string& filename)
{
    std::ifstream input(filename, std::ios::binary);

    if (!input) {
        throw std::runtime_error(
            "read_vectors: не удалось открыть файл '" + filename + "'"
        );
    }

    std::uint32_t vector_count = 0;
    read_value(input, vector_count, filename);

    DataVectors vectors;
    vectors.reserve(vector_count);

    for (std::uint32_t i = 0; i < vector_count; ++i) {
        std::uint32_t vector_size = 0;
        read_value(input, vector_size, filename);

        std::vector<double> values(vector_size);

        if (!values.empty()) {
            const auto bytes = values.size() * sizeof(double);

            if (bytes > static_cast<std::size_t>(
                            std::numeric_limits<std::streamsize>::max())) {
                throw std::runtime_error(
                    "read_vectors: вектор слишком велик в файле '" +
                    filename + "'"
                );
            }

            input.read(reinterpret_cast<char*>(values.data()),
                       static_cast<std::streamsize>(bytes));

            if (!input ||
                input.gcount() != static_cast<std::streamsize>(bytes)) {
                throw std::runtime_error(
                    "read_vectors: неполные данные в файле '" +
                    filename + "'"
                );
            }
        }

        vectors.push_back(std::move(values));
    }

    if (input.peek() != std::char_traits<char>::eof()) {
        throw std::runtime_error(
            "read_vectors: после ожидаемых данных найдены лишние байты в файле '" +
            filename + "'"
        );
    }

    if (input.bad()) {
        throw std::runtime_error(
            "read_vectors: ошибка при чтении файла '" + filename + "'"
        );
    }

    return vectors;
}

void write_results(const std::string& filename,
                   const std::vector<double>& results)
{
    if (results.size() > std::numeric_limits<std::uint32_t>::max()) {
        throw std::runtime_error(
            "write_results: слишком много результатов для файла '" +
            filename + "'"
        );
    }

    std::ofstream output(filename);

    if (!output) {
        throw std::runtime_error(
            "write_results: не удалось открыть файл '" + filename + "'"
        );
    }

    output.imbue(std::locale::classic());
    output << results.size();

    output.precision(std::numeric_limits<double>::max_digits10);
    for (const double value : results) {
        output << ' ' << value;
    }
    output << '\n';

    if (!output) {
        throw std::runtime_error(
            "write_results: ошибка записи файла '" + filename + "'"
        );
    }
}
