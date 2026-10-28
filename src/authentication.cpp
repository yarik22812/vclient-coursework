#include "authentication.hpp"

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>

#include <stdexcept>
#include <string>

namespace {

std::string bytes_to_hex(const CryptoPP::byte* data, std::size_t size)
{
    std::string result;
    CryptoPP::StringSource(
        data,
        size,
        true,
        new CryptoPP::HexEncoder(
            new CryptoPP::StringSink(result),
            true // заглавные шестнадцатеричные цифры
        )
    );
    return result;
}

std::string make_salt()
{
    CryptoPP::byte salt[8]; // 64 случайных бита
    CryptoPP::AutoSeededRandomPool random;
    random.GenerateBlock(salt, sizeof(salt));
    return bytes_to_hex(salt, sizeof(salt)); // ровно 16 символов
}

std::string make_sha256_hex(const std::string& text)
{
    CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
    CryptoPP::SHA256 hash;

    hash.CalculateDigest(
        digest,
        reinterpret_cast<const CryptoPP::byte*>(text.data()),
        text.size()
    );

    return bytes_to_hex(digest, sizeof(digest));
}

} // namespace

std::string make_auth_message(const std::string& login,
                              const std::string& password)
{
    if (login.empty() || password.empty()) {
        throw std::invalid_argument(
            "make_auth_message: логин и пароль не должны быть пустыми"
        );
    }

    const std::string salt = make_salt();
    const std::string hash = make_sha256_hex(salt + password);

    return login + salt + hash;
}
