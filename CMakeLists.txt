cmake_minimum_required(VERSION 3.16)

project(vclient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_path(CRYPTOPP_INCLUDE_DIR cryptopp/sha.h REQUIRED)
find_library(CRYPTOPP_LIBRARY NAMES cryptopp crypto++ REQUIRED)

add_executable(vclient
    src/main.cpp
    src/authentication.cpp
    src/credentials.cpp
    src/data_io.cpp
    src/client_options.cpp
    src/tcp_client.cpp
)

target_include_directories(vclient PRIVATE
    include
    ${CRYPTOPP_INCLUDE_DIR}
)

target_link_libraries(vclient PRIVATE
    ${CRYPTOPP_LIBRARY}
)

target_compile_options(vclient PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
