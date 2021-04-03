project(test_common)

add_executable(${PROJECT_NAME}
        test_common.cpp
        )

target_link_libraries(${PROJECT_NAME} gtest_main common)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
