#include "client_options.hpp"

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

ClientOptions parse(std::vector<std::string> arguments)
{
    std::vector<char*> argv;
    for (auto& argument : arguments) {
        argv.push_back(argument.data());
    }

    return parse_options(static_cast<int>(argv.size()), argv.data());
}

} // namespace

int main()
{
    const auto defaults = parse({
        "vclient", "-a", "127.0.0.1",
        "-i", "input.bin", "-o", "result.txt"
    });

    assert(defaults.server_address == "127.0.0.1");
    assert(defaults.port == 33333);
    assert(defaults.input_file == "input.bin");
    assert(defaults.output_file == "result.txt");
    assert(defaults.credentials_file == "~/.config/vclient.conf");

    const auto custom = parse({
        "vclient", "--address", "localhost",
        "--port", "12345",
        "--input", "data.bin",
        "--output", "out.txt",
        "--credentials", "user.conf"
    });

    assert(custom.server_address == "localhost");
    assert(custom.port == 12345);
    assert(custom.credentials_file == "user.conf");

    const auto help = parse({"vclient", "-h"});
    assert(help.help_requested);

    bool missing_address_threw = false;
    try {
        parse({"vclient", "-i", "input.bin", "-o", "result.txt"});
    } catch (const std::runtime_error&) {
        missing_address_threw = true;
    }
    assert(missing_address_threw);

    bool invalid_port_threw = false;
    try {
        parse({
            "vclient", "-a", "localhost",
            "-p", "70000",
            "-i", "input.bin",
            "-o", "result.txt"
        });
    } catch (const std::runtime_error&) {
        invalid_port_threw = true;
    }
    assert(invalid_port_threw);
}
