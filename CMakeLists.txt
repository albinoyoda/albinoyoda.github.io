cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(wow_simulator)

include(CTest)
enable_testing()

add_subdirectory(google_tests/lib)
add_subdirectory(local_build)
add_subdirectory(simulator)
add_subdirectory(statistics)
add_subdirectory(wow_library)
