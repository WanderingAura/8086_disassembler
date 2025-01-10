cmake_minimum_required(VERSION 3.5)
project(disassembler)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE sources  src/*.cpp)


add_executable(dis86 ${sources})

set_target_properties(dis86 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG $(CMAKE_SOURCE_DIR)/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE $(CMAKE_SOURCE_DIR)/bin/Release)

target_include_directories(dis86 PRIVATE src/)