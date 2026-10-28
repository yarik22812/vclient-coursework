#include "auth.hpp"

#include <array>
#include <stdexcept>

#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>

namespace auth {
namespace {

constexpr char HEX[] = "0123456789ABCDEF";

template <std::size_t N>
std::string to_hex(const std::array<CryptoPP::byte, N>& bytes)
{
    std::string result;
    result.reserve(N * 2);

    for (const auto byte : bytes) {
        const unsigned value = static_cast<unsigned>(byte);
        result.push_back(HEX[(value >> 4) & 0x0F]);
        result.push_back(HEX[value & 0x0F]);
    }

    return result;
}

} // namespace

std::string make_salt()
{
    std::array<CryptoPP::byte, 8> random_bytes{};

    CryptoPP::AutoSeededRandomPool generator;
    generator.GenerateBlock(random_bytes.data(), random_bytes.size());

    // 8 байт = 64 бита = ровно 16 шестнадцатеричных цифр.
    return to_hex(random_bytes);
}

std::string sha256_hex(std::string_view data)
{
    CryptoPP::SHA256 sha256;
    std::array<CryptoPP::byte, CryptoPP::SHA256::DIGESTSIZE> digest{};

    sha256.CalculateDigest(
        digest.data(),
        reinterpret_cast<const CryptoPP::byte*>(data.data()),
        data.size()
    );

    return to_hex(digest);
}

std::string password_hash(std::string_view salt16,
                          std::string_view password)
{
    if (!is_valid_salt(salt16)) {
        throw std::invalid_argument(
            "password_hash: соль должна содержать 16 hex-символов"
        );
    }

    std::string input;
    input.reserve(salt16.size() + password.size());
    input.append(salt16);
    input.append(password);

    return sha256_hex(input);
}

std::string make_auth_message(std::string_view login,
                              std::string_view password,
                              std::string_view salt16)
{
    if (login.empty()) {
        throw std::invalid_argument(
            "make_auth_message: логин не должен быть пустым"
        );
    }

    const std::string hash = password_hash(salt16, password);

    std::string message;
    message.reserve(login.size() + salt16.size() + hash.size());
    message.append(login);
    message.append(salt16);
    message.append(hash);

    return message;
}

bool is_valid_salt(std::string_view salt16)
{
    if (salt16.size() != 16) {
        return false;
    }

    for (const char c : salt16) {
        const bool is_digit = c >= '0' && c <= '9';
        const bool is_upper_hex = c >= 'A' && c <= 'F';

        if (!is_digit && !is_upper_hex) {
            return false;
        }
    }

    return true;
}

} // namespace auth
