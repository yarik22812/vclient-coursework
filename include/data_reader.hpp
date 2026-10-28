#pragma once

#include <string>
#include <vector>

using Vectors = std::vector<std::vector<double>>;

/// Читает двоичный файл с числовыми векторами.
/// Формат: число векторов uint32_t, затем для каждого вектора
/// его размер uint32_t и значения double.
Vectors read_vectors(const std::string& file_name);
