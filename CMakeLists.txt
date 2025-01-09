cmake_minimum_required(VERSION 3.0)
project(disassembler)

file(GLOB_RECURSE sources  src/*.cpp)

add_executable(dis86 ${sources})

set_target_properties(dis86 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
)

target_include_directories(dis86 PRIVATE src/)