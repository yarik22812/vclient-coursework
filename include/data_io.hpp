#pragma once

#include <cstdint>
#include <string>
#include <vector>

using DataVectors = std::vector<std::vector<double>>;

DataVectors read_vectors(const std::string& filename);
void write_results(const std::string& filename,
                   const std::vector<double>& results);
