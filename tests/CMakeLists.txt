

add_executable(dis86_test test_mov.cpp)
target_link_libraries(dis86_test gtest_main)
include(GoogleTest)
gtest_discover_tests(dis86_test)
