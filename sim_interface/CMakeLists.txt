cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project(sim_interface)

add_library(
        ${PROJECT_NAME}
        source/sim_interface.cpp
        source/sim_interface_mult.cpp
)

target_link_libraries(${PROJECT_NAME} simulator wow_library)

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})