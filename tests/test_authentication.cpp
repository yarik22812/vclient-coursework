#include "authentication.hpp"

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include <cassert>
#include <cctype>
#include <string>

int main()
{
    const std::string login = "test_user";
    const std::string password = "test_password";

    const std::string message = make_auth_message(login, password);

    // LOGIN + 16 символов соли + 64 символа SHA-256
    assert(message.size() == login.size() + 16 + 64);
    assert(message.substr(0, login.size()) == login);

    const std::string salt = message.substr(login.size(), 16);
    const std::string received_hash = message.substr(login.size() + 16);

    for (unsigned char c : salt + received_hash) {
        assert(std::isdigit(c) || (c >= 'A' && c <= 'F'));
    }

    std::string expected_hash;
    CryptoPP::SHA256 sha256;
    CryptoPP::StringSource(
        salt + password,
        true,
        new CryptoPP::HashFilter(
            sha256,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(expected_hash),
                true)));

    assert(received_hash == expected_hash);
}
