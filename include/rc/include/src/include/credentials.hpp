#pragma once

#include <string>

struct Credentials {
    std::string login;
    std::string password;
};

Credentials read_credentials(const std::string& file_name);
