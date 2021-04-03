project(test_wow_library)

add_executable(${PROJECT_NAME} test_attributes.cpp)

target_link_libraries(${PROJECT_NAME} gtest_main wow_library)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
