cmake_minimum_required(VERSION 3.15)
project(wow_simulator)

set(CMAKE_CXX_STANDARD 14)

add_executable(wow_simulator
        main.cpp
        Combat_simulator.cpp
        Stats.cpp
        Item.cpp
        Character.cpp
        Enchant.cpp
        Armory.hpp)

#target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)
target_compile_options(${PROJECT_NAME} PRIVATE -O2)
#target_compile_options(${PROJECT_NAME} PRIVATE -O3 -march=native)