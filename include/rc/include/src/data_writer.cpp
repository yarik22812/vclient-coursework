#include "data_writer.hpp"

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>

void write_results(const std::string& file_name,
                   const std::vector<double>& results)
{
    if (results.size() > std::numeric_limits<std::uint32_t>::max()) {
        throw std::runtime_error(
            "write_results: слишком много результатов для файла '" +
            file_name + "'"
        );
    }

    std::ofstream file(file_name);

    if (!file) {
        throw std::runtime_error(
            "write_results: не удалось открыть файл '" + file_name +
            "' для записи"
        );
    }

    file << results.size();

    file << std::setprecision(std::numeric_limits<double>::max_digits10);
    for (double value : results) {
        file << ' ' << value;
    }
    file << '\n';

    if (!file) {
        throw std::runtime_error(
            "write_results: ошибка записи в файл '" + file_name + "'"
        );
    }
}
