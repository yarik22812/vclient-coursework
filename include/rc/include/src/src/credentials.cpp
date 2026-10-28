#include "credentials.hpp"

#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>

namespace {

std::string expand_home(const std::string& path)
{
    if (path.size() >= 2 && path[0] == '~' && path[1] == '/') {
        const char* home = std::getenv("HOME");
        if (home == nullptr || *home == '\0') {
            throw std::runtime_error(
                "read_credentials: не задана переменная HOME"
            );
        }
        return std::string(home) + path.substr(1);
    }

    return path;
}

} // namespace

Credentials read_credentials(const std::string& file_name)
{
    const std::string path = expand_home(file_name);
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error(
            "read_credentials: не удалось открыть файл '" + path + "'"
        );
    }

    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error(
            "read_credentials: файл '" + path + "' пуст"
        );
    }

    // Убираем возврат каретки из файла с окончаниями строк Windows.
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    const std::size_t separator = line.find(':');
    if (separator == std::string::npos ||
        separator == 0 ||
        separator + 1 >= line.size()) {
        throw std::runtime_error(
            "read_credentials: ожидается строка LOGIN:PASSWORD в файле '" +
            path + "'"
        );
    }

    return {
        line.substr(0, separator),
        line.substr(separator + 1)
    };
}
