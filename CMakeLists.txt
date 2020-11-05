cmake_minimum_required(VERSION 3.14)
project(wow_simulator)

set(CMAKE_CXX_STANDARD 14)
if (CMAKE_BUILD_TYPE MATCHES Debug)

    message("CMAKE IN DEBUG MODE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -Werror -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Werror -Wall -Wextra")

else ()

    message("CMAKE IN RELEASE MODE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

endif ()

include_directories(wow_library/include)

add_library(
        wow_lib STATIC
        wow_library/source/Combat_simulator.cpp
        wow_library/source/Item.cpp
        wow_library/source/Character.cpp
        wow_library/source/weapon_sim.cpp
        wow_library/source/Attributes.cpp
        wow_library/source/Armory.cpp
        wow_library/source/sim_interface.cpp
        wow_library/source/sim_interface_mult.cpp
        wow_library/source/Statistics.cpp
        wow_library/source/damage_sources.cpp
        wow_library/source/Item_optimizer.cpp
)


if (EMSCRIPTEN)

    add_executable(
            wow_interface
            interface/src/entry_point.cpp
    )

    target_link_libraries(wow_interface wow_lib)
    set_target_properties(wow_interface PROPERTIES LINK_FLAGS "-s ERROR_ON_UNDEFINED_SYMBOLS=0 -s DEMANGLE_SUPPORT=1 -s TOTAL_MEMORY=640MB -s ASSERTIONS=1 --bind")

else ()

    # Main executable when running offline
    add_executable(wow_web main_web_code.cpp)
    target_link_libraries(wow_web wow_lib)

endif ()
