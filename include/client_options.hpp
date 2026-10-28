#pragma once

#include <cstdint>
#include <string>
#include <string_view>

struct ClientOptions {
    std::string server_address;
    std::uint16_t port = 33333;
    std::string input_file;
    std::string output_file;
    std::string credentials_file = "~/.config/vclient.conf";
    bool help_requested = false;
};

ClientOptions parse_options(int argc, char* argv[]);
std::string usage(std::string_view program_name);
