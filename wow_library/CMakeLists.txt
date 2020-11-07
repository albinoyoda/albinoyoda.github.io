set(CMAKE_CXX_STANDARD 14)
project(wow_library)

if (CMAKE_BUILD_TYPE MATCHES Debug)

    message("CMAKE IN DEBUG MODE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -Werror -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Werror -Wall -Wextra")
    #    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

else ()

    message("CMAKE IN RELEASE MODE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

endif ()

include_directories(include)

add_library(
        wow_library STATIC
        source/Combat_simulator.cpp
        source/Item.cpp
        source/Character.cpp
        source/weapon_sim.cpp
        source/Attributes.cpp
        source/Armory.cpp
        source/sim_interface.cpp
        source/sim_interface_mult.cpp
        source/Statistics.cpp
        source/damage_sources.cpp
        source/Item_optimizer.cpp
        source/Helper_functions.cpp
)

