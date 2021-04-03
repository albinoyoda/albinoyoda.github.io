project(test_wow_library)

add_executable(test_wow_library test_attributes.cpp)

target_link_libraries(test_wow_library gtest gtest_main wow_library)

