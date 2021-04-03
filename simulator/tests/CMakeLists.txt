project(test_simulator)

add_executable(test_simulator test_ap_estimation.cpp test_item_optimizer.cpp)

target_link_libraries(test_simulator gtest gtest_main wow_library simulator)

add_test(NAME simulator_tests COMMAND test_simulator)
