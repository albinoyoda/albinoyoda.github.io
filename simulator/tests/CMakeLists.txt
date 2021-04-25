project(test_simulator)

add_executable(${PROJECT_NAME}
        test_ap_estimation.cpp
        test_use_effects.cpp
        test_simulator.cpp
        simulation_fixture.cpp
        )

target_link_libraries(${PROJECT_NAME} gtest_main wow_library simulator statistics)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
