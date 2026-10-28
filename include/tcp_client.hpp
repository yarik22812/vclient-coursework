#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

class TcpClient {
public:
    TcpClient() = default;
    ~TcpClient();

    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    void connect_to(const std::string& host, std::uint16_t port);
    void send_all(const void* data, std::size_t size);
    void receive_exact(void* data, std::size_t size);
    void close() noexcept;

private:
    int socket_ = -1;
};
