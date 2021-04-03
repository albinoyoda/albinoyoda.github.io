project(simulator_tests)

add_executable(simulator_tests test_ap_estimation.cpp test_item_optimizer.cpp)

target_link_libraries(simulator_tests gtest gtest_main wow_library simulator)
