cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project(wow_simulator)

include(FetchContent)

if (NOT EMSCRIPTEN)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.10.0
    )
    FetchContent_MakeAvailable(googletest)
    enable_testing()
endif ()


if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building ${CMAKE_PROJECT_NAME} in Debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wunused -std=c++17 -pedantic")
else ()
    message("Building ${CMAKE_PROJECT_NAME} in Release mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2")
endif ()

add_subdirectory(simulator)
add_subdirectory(statistics)
add_subdirectory(wow_library)
add_subdirectory(sim_interface)
add_subdirectory(common)
add_subdirectory(item_optimizer)

if (NOT EMSCRIPTEN)
    add_subdirectory(local_build)
endif ()

if (EMSCRIPTEN)
    add_subdirectory(website)
endif ()
