project(test_statistics)

add_executable(test_statistics test_statistics.cpp)

target_link_libraries(test_statistics gtest gtest_main statistics)

add_test(NAME statistics_tests COMMAND test_statistics)