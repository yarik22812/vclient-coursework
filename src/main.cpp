#include "authentication.hpp"
#include "client_options.hpp"
#include "credentials.hpp"
#include "data_io.hpp"
#include "tcp_client.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void send_authentication(TcpClient& client, const std::string& message)
{
    client.send_all(message.data(), message.size());

    // По заданию сервер отвечает "OK" или "ERR".
    char response[2]{};
    client.receive_exact(response, sizeof(response));

    if (response[0] == 'O' && response[1] == 'K') {
        return;
    }

    if (response[0] == 'E' && response[1] == 'R') {
        char third = '\0';
        client.receive_exact(&third, sizeof(third));

        if (third == 'R') {
            throw std::runtime_error("аутентификация отклонена сервером: ERR");
        }
    }

    throw std::runtime_error("сервер вернул неизвестный ответ аутентификации");
}

void send_vectors_and_receive_results(
    TcpClient& client,
    const DataVectors& vectors,
    std::vector<double>& results)
{
    if (vectors.size() > std::numeric_limits<std::uint32_t>::max()) {
        throw std::runtime_error("слишком много векторов для передачи");
    }

    const auto vector_count = static_cast<std::uint32_t>(vectors.size());
    client.send_all(&vector_count, sizeof(vector_count));

    results.clear();
    results.reserve(vectors.size());

    for (const auto& vector : vectors) {
        if (vector.size() > std::numeric_limits<std::uint32_t>::max()) {
            throw std::runtime_error("слишком много элементов в одном векторе");
        }

        const auto vector_size = static_cast<std::uint32_t>(vector.size());
        client.send_all(&vector_size, sizeof(vector_size));

        if (!vector.empty()) {
            client.send_all(vector.data(), vector.size() * sizeof(double));
        }

        double result{};
        client.receive_exact(&result, sizeof(result));
        results.push_back(result);
    }
}

} // namespace

int main(int argc, char* argv[])
{
    try {
        if (argc == 1) {
            std::cout << usage(argv[0]);
            return 0;
        }

        const ClientOptions options = parse_options(argc, argv);

        if (options.help_requested) {
            std::cout << usage(argv[0]);
            return 0;
        }

        const Credentials credentials =
            read_credentials(options.credentials_file);

        const DataVectors vectors = read_vectors(options.input_file);

        TcpClient client;
        client.connect_to(options.server_address, options.port);

        const std::string auth_message =
            make_auth_message(credentials.login, credentials.password);
        send_authentication(client, auth_message);

        std::vector<double> results;
        send_vectors_and_receive_results(client, vectors, results);

        client.close();
        write_results(options.output_file, results);

        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
