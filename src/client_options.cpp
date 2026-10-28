#include "client_options.hpp"

#include <cstdlib>
#include <stdexcept>
#include <string>

std::string usage(std::string_view program_name)
{
    return
        "Использование: " + std::string(program_name) +
        " --address АДРЕС --input ФАЙЛ --output ФАЙЛ [параметры]\n"
        "Параметры:\n"
        "  -a, --address АДРЕС     Адрес сервера (обязательный)\n"
        "  -p, --port ПОРТ         Порт сервера (по умолчанию 33333)\n"
        "  -i, --input ФАЙЛ        Файл исходных данных (обязательный)\n"
        "  -o, --output ФАЙЛ       Файл результатов (обязательный)\n"
        "  -c, --credentials ФАЙЛ  Файл логина и пароля\n"
        "                          (по умолчанию ~/.config/vclient.conf)\n"
        "  -h, --help              Показать справку\n";
}

ClientOptions parse_options(int argc, char* argv[])
{
    ClientOptions options;

    if (argc <= 1) {
        options.help_requested = true;
        return options;
    }

    auto value_after = [&](int& index, const std::string& option) -> std::string {
        if (index + 1 >= argc || argv[index + 1][0] == '-') {
            throw std::invalid_argument(
                "parse_options: отсутствует значение параметра " + option
            );
        }
        return argv[++index];
    };

    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];

        if (argument == "-h" || argument == "--help") {
            options.help_requested = true;
            return options;
        } else if (argument == "-a" || argument == "--address") {
            options.server_address = value_after(i, argument);
        } else if (argument == "-p" || argument == "--port") {
            const std::string value = value_after(i, argument);
            std::size_t parsed = 0;
            unsigned long port = 0;

            try {
                port = std::stoul(value, &parsed);
            } catch (...) {
                throw std::invalid_argument(
                    "parse_options: некорректный порт '" + value + "'"
                );
            }

            if (parsed != value.size() || port == 0 || port > 65535) {
                throw std::invalid_argument(
                    "parse_options: порт должен быть числом от 1 до 65535"
                );
            }

            options.port = static_cast<std::uint16_t>(port);
        } else if (argument == "-i" || argument == "--input") {
            options.input_file = value_after(i, argument);
        } else if (argument == "-o" || argument == "--output") {
            options.output_file = value_after(i, argument);
        } else if (argument == "-c" || argument == "--credentials") {
            options.credentials_file = value_after(i, argument);
        } else {
            throw std::invalid_argument(
                "parse_options: неизвестный параметр '" + argument + "'"
            );
        }
    }

    if (options.server_address.empty()) {
        throw std::invalid_argument(
            "parse_options: не задан обязательный адрес сервера"
        );
    }
    if (options.input_file.empty()) {
        throw std::invalid_argument(
            "parse_options: не задан обязательный входной файл"
        );
    }
    if (options.output_file.empty()) {
        throw std::invalid_argument(
            "parse_options: не задан обязательный файл результатов"
        );
    }

    return options;
}
