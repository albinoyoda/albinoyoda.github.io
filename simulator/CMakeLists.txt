set(CMAKE_CXX_STANDARD 14)
project(simulator)

include_directories(include)

add_library(
        simulator
        source/Combat_simulator.cpp
        source/weapon_sim.cpp
        source/sim_interface.cpp
        source/sim_interface_mult.cpp
        source/damage_sources.cpp
        source/Item_optimizer.cpp
        source/Helper_functions.cpp
        source/Helper_functions.cpp
)

target_include_directories(simulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(simulator statistics wow_library)

if (NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif ()