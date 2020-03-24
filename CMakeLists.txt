cmake_minimum_required(VERSION 3.15)
project(wow_simulator)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra")
set(CMAKE_CXX_FLAGS "-O2")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")

add_library(wow_lib SHARED
        wow_library/source/Combat_simulator.cpp
        wow_library/source/Stats.cpp
        wow_library/source/Item.cpp
        wow_library/source/Character.cpp
        wow_library/source/Enchant.cpp
        wow_library/include/Armory.hpp
        )

ADD_EXECUTABLE(wow_simulator main_simulator.cpp)
target_link_libraries(wow_simulator wow_lib)
#target_compile_options(wow_simulator -Werror -Wall -Wextra)

ADD_EXECUTABLE(wow_optimizer main_optimizer.cpp)
target_link_libraries(wow_optimizer wow_lib)


#target_compile_options(${PROJECT_NAME} PRIVATE -O2)