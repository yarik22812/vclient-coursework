#include "credentials.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>

namespace {

const std::string valid_path = "test_credentials_valid.tmp";
const std::string crlf_path = "test_credentials_crlf.tmp";
const std::string empty_path = "test_credentials_empty.tmp";
const std::string malformed_path = "test_credentials_malformed.tmp";
const std::string missing_path = "test_credentials_missing.tmp";

void write_file(const std::string& path, const std::string& contents)
{
    std::ofstream file(path, std::ios::binary);
    assert(file.is_open());

    file << contents;
    assert(file.good());
}

void expect_error(const std::string& path)
{
    bool threw = false;

    try {
        (void)read_credentials(path);
    } catch (const std::runtime_error&) {
        threw = true;
    }

    assert(threw);
}

void test_read_valid_credentials()
{
    write_file(valid_path, "test_user:test_password\n");

    const Credentials credentials = read_credentials(valid_path);
    assert(credentials.login == "test_user");
    assert(credentials.password == "test_password");
}

void test_read_credentials_with_crlf()
{
    write_file(crlf_path, "test_user:test_password\r\n");

    const Credentials credentials = read_credentials(crlf_path);
    assert(credentials.login == "test_user");
    assert(credentials.password == "test_password");
}

void test_reject_missing_file()
{
    std::remove(missing_path.c_str());
    expect_error(missing_path);
}

void test_reject_empty_file()
{
    write_file(empty_path, "");
    expect_error(empty_path);
}

void test_reject_malformed_credentials()
{
    write_file(malformed_path, "test_user_without_password\n");
    expect_error(malformed_path);
}

} // namespace

int main()
{
    test_read_valid_credentials();
    test_read_credentials_with_crlf();
    test_reject_missing_file();
    test_reject_empty_file();
    test_reject_malformed_credentials();

    std::remove(valid_path.c_str());
    std::remove(crlf_path.c_str());
    std::remove(empty_path.c_str());
    std::remove(malformed_path.c_str());
    std::remove(missing_path.c_str());
}
