#include "data_io.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

const std::string input_path = "test_data_io_input.bin";
const std::string output_path = "test_data_io_output.txt";
const std::string broken_path = "test_data_io_broken.bin";

void write_binary_input(const std::string& path)
{
    std::ofstream file(path, std::ios::binary);
    const std::uint32_t count = 2;
    const std::uint32_t size1 = 3;
    const std::uint32_t size2 = 2;
    const double vector1[] = {1.25, -2.5, 3.75};
    const double vector2[] = {10.0, 20.5};

    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    file.write(reinterpret_cast<const char*>(&size1), sizeof(size1));
    file.write(reinterpret_cast<const char*>(vector1), sizeof(vector1));
    file.write(reinterpret_cast<const char*>(&size2), sizeof(size2));
    file.write(reinterpret_cast<const char*>(vector2), sizeof(vector2));

    assert(file.good());
}

void test_read_vectors()
{
    write_binary_input(input_path);

    const DataVectors expected{
        {1.25, -2.5, 3.75},
        {10.0, 20.5}
    };

    const DataVectors actual = read_vectors(input_path);
    assert(actual == expected);
}

void test_read_empty_dataset()
{
    std::ofstream file(input_path, std::ios::binary);
    const std::uint32_t count = 0;
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    file.close();

    assert(read_vectors(input_path).empty());
}

void test_reject_truncated_file()
{
    std::ofstream file(broken_path, std::ios::binary);
    const std::uint32_t count = 1;
    const std::uint32_t size = 2;
    const double only_one_value = 4.0;

    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    file.write(
        reinterpret_cast<const char*>(&only_one_value),
        sizeof(only_one_value));
    file.close();

    bool threw = false;
    try {
        (void)read_vectors(broken_path);
    } catch (const std::exception&) {
        threw = true;
    }
    assert(threw);
}

void test_write_results()
{
    const std::vector<double> expected{1.25, -3.5, 0.125};
    write_results(output_path, expected);

    std::ifstream file(output_path);
    std::size_t count = 0;
    file >> count;
    assert(count == expected.size());

    for (double value : expected) {
        double actual = 0.0;
        file >> actual;
        assert(file.good() || file.eof());
        assert(std::abs(actual - value) < 1e-12);
    }
}

} // namespace

int main()
{
    test_read_vectors();
    test_read_empty_dataset();
    test_reject_truncated_file();
    test_write_results();

    std::remove(input_path.c_str());
    std::remove(output_path.c_str());
    std::remove(broken_path.c_str());
}
