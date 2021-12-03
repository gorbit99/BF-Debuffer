cmake_minimum_required(VERSION 3.15)

project (bf-debugger
    LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(./src)
