project(test_simulator)

add_executable(${PROJECT_NAME}
        test_ap_estimation.cpp
        test_use_effects.cpp
        test_simulator.cpp
        )

target_link_libraries(${PROJECT_NAME} gtest_main wow_library simulator)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
