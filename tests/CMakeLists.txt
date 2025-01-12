
add_executable(dis86_test 
    test_mov.cpp 
    ../src/dis86_instruction.cpp
    ../src/dis86_instruction_stream.cpp
    ../src/dis86_inst_format.cpp
)
target_include_directories(dis86_test PRIVATE ../src/)
target_link_libraries(dis86_test gtest_main)
include(GoogleTest)
gtest_discover_tests(dis86_test)
