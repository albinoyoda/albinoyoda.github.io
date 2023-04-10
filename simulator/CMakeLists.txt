set(CMAKE_CXX_STANDARD 17)
project(simulator)

add_library(
        ${PROJECT_NAME}
        source/Combat_simulator.cpp
        source/weapon_sim.cpp
        source/damage_sources.cpp
        source/use_effects_schedule.cpp
        source/combat_simulator_config.cpp
        source/hit_tables.cpp
        source/spells.cpp
        source/use_effects.cpp
        source/time_keeper.cpp
        source/cooldown_manager.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
        statistics
        wow_library
        sim_interface
        common)

if (NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif ()