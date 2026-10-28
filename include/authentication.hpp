#pragma once

#include <string>

/// Формирует сообщение аутентификации: LOGIN || SALT16 || HASH16.
std::string make_auth_message(const std::string& login,
                              const std::string& password);
