cmake_minimum_required(VERSION 3.14)
project(wow_simulator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra")
#set(CMAKE_CXX_FLAGS "-O2")

set(LIB_TYPE STATIC)

include_directories(wow_library/include)

add_library(wow_lib SHARED
        wow_library/source/Combat_simulator.cpp
        wow_library/source/Item.cpp
        wow_library/source/Character.cpp
        wow_library/source/weapon_sim.cpp
        wow_library/source/Attributes.cpp
        wow_library/source/Armory.cpp
        wow_library/source/sim_interface.cpp
        wow_library/source/Statistics.cpp
        wow_library/source/damage_sources.cpp)

ADD_EXECUTABLE(wow_simulator main_simulator.cpp)
target_link_libraries(wow_simulator wow_lib)

#ADD_EXECUTABLE(wow_optimizer main_optimizer.cpp)
#target_link_libraries(wow_optimizer wow_lib)

ADD_EXECUTABLE(wow_stat_weights main_stat_weights.cpp)
target_link_libraries(wow_stat_weights wow_lib)

ADD_EXECUTABLE(wow_web main_web_code.cpp)
target_link_libraries(wow_web wow_lib)

IF (EMSCRIPTEN)
    add_executable(wow_interface
            interface/src/entry_point.cpp
            )

    target_link_libraries(wow_interface wow_lib)
    set_target_properties(wow_interface PROPERTIES LINK_FLAGS "-s ERROR_ON_UNDEFINED_SYMBOLS=0 -s DEMANGLE_SUPPORT=1 -s TOTAL_MEMORY=1280MB -s --bind") #


ENDIF ()
