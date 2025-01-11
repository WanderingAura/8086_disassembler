cmake_minimum_required(VERSION 3.5)
project(disassembler)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE sources  src/*.cpp)


add_executable(dis86 ${sources})

set_target_properties(dis86 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
)

include(Ctest)
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG $(CMAKE_SOURCE_DIR)/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE $(CMAKE_SOURCE_DIR)/bin/Release)

target_include_directories(dis86 PRIVATE src/)