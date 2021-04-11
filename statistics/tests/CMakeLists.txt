project(test_statistics)

add_executable(${PROJECT_NAME}
        test_statistics.cpp
        test_distribution.cpp
        )

target_link_libraries(${PROJECT_NAME} gtest_main statistics)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})