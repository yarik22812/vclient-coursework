#pragma once

#include <string>
#include <string_view>

namespace auth {

/// Формирует случайную 64-битную соль как 16 заглавных hex-символов.
std::string make_salt();

/// Возвращает SHA-256 от строки в виде 64 заглавных hex-символов.
std::string sha256_hex(std::string_view data);

/// Вычисляет SHA-256(SALT16 || PASSWORD).
std::string password_hash(std::string_view salt16,
                          std::string_view password);

/// Формирует LOGIN || SALT16 || HASH.
std::string make_auth_message(std::string_view login,
                              std::string_view password,
                              std::string_view salt16);

/// Проверяет, что соль содержит ровно 16 допустимых hex-символов.
bool is_valid_salt(std::string_view salt16);

} // namespace auth
