project(test_item_optimizer)

add_executable(${PROJECT_NAME}
        test_item_optimizer.cpp
        )

target_link_libraries(${PROJECT_NAME} gtest_main wow_library sim_interface)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
