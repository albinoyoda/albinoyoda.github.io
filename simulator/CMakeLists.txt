set(CMAKE_CXX_STANDARD 17)
project(simulator)

add_library(
        ${PROJECT_NAME}
        source/Combat_simulator.cpp
        source/weapon_sim.cpp
        source/damage_sources.cpp
        source/Item_optimizer.cpp
        source/Helper_functions.cpp
        source/Helper_functions.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} statistics wow_library sim_interface)

if (NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif ()