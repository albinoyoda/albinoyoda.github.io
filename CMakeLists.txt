cmake_minimum_required(VERSION 3.15)
project(wow_simulator)

set(CMAKE_CXX_STANDARD 14)

add_executable(wow_simulator main.cpp Combat_simulator.cpp Combat_simulator.hpp Stats.cpp Stats.hpp Item.cpp Item.hpp Character.cpp Character.hpp Enchant.cpp Enchant.hpp)