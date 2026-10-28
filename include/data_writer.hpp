#pragma once

#include <string>
#include <vector>

/// Записывает результаты одной строкой:
/// количество результатов, затем значения через пробел.
void write_results(const std::string& file_name,
                   const std::vector<double>& results);
