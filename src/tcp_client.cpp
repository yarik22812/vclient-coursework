#include "tcp_client.hpp"

#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <system_error>
#include <sys/socket.h>
#include <unistd.h>

TcpClient::~TcpClient()
{
    close();
}

void TcpClient::connect_to(const std::string& host, std::uint16_t port)
{
    close();

    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo* addresses = nullptr;
    const std::string service = std::to_string(port);

    const int status = getaddrinfo(host.c_str(), service.c_str(),
                                   &hints, &addresses);
    if (status != 0) {
        throw std::runtime_error(
            "connect_to: не удалось определить адрес сервера '" + host +
            "': " + gai_strerror(status));
    }

    int connected_socket = -1;
    int last_error = 0;

    for (addrinfo* address = addresses; address != nullptr;
         address = address->ai_next) {
        const int candidate = ::socket(address->ai_family,
                                       address->ai_socktype,
                                       address->ai_protocol);
        if (candidate == -1) {
            last_error = errno;
            continue;
        }

        if (::connect(candidate, address->ai_addr, address->ai_addrlen) == 0) {
            connected_socket = candidate;
            break;
        }

        last_error = errno;
        ::close(candidate);
    }

    freeaddrinfo(addresses);

    if (connected_socket == -1) {
        throw std::system_error(
            last_error, std::generic_category(),
            "connect_to: ошибка соединения с " + host + ":" + service);
    }

    socket_ = connected_socket;
}

void TcpClient::send_all(const void* data, std::size_t size)
{
    if (socket_ == -1) {
        throw std::runtime_error("send_all: TCP-соединение не установлено");
    }

    const auto* bytes = static_cast<const char*>(data);
    std::size_t sent = 0;

    while (sent < size) {
        const ssize_t count =
            ::send(socket_, bytes + sent, size - sent, MSG_NOSIGNAL);

        if (count == -1 && errno == EINTR) {
            continue;
        }
        if (count == -1) {
            throw std::system_error(errno, std::generic_category(),
                                    "send_all: ошибка отправки данных");
        }
        if (count == 0) {
            throw std::runtime_error("send_all: соединение закрыто сервером");
        }

        sent += static_cast<std::size_t>(count);
    }
}

void TcpClient::receive_exact(void* data, std::size_t size)
{
    if (socket_ == -1) {
        throw std::runtime_error("receive_exact: TCP-соединение не установлено");
    }

    auto* bytes = static_cast<char*>(data);
    std::size_t received = 0;

    while (received < size) {
        const ssize_t count =
            ::recv(socket_, bytes + received, size - received, 0);

        if (count == -1 && errno == EINTR) {
            continue;
        }
        if (count == -1) {
            throw std::system_error(errno, std::generic_category(),
                                    "receive_exact: ошибка приёма данных");
        }
        if (count == 0) {
            throw std::runtime_error(
                "receive_exact: сервер закрыл соединение до получения всех данных");
        }

        received += static_cast<std::size_t>(count);
    }
}

void TcpClient::close() noexcept
{
    if (socket_ != -1) {
        ::close(socket_);
        socket_ = -1;
    }
}
