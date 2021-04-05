cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(wow_simulator)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.10.0
)
FetchContent_MakeAvailable(googletest)

#FetchContent_Declare(
#        emscripten
#        GIT_REPOSITORY https://github.com/emscripten-core/emscripten.git
#        GIT_TAG        main
#)
#FetchContent_MakeAvailable(emscripten)

#include(CTest)
enable_testing()

add_compile_options(-Wall -Wextra -pedantic -Werror)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building ${CMAKE_PROJECT_NAME} in Debug mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wunused -std=c++14 -pedantic")
else ()
    message("Building ${CMAKE_PROJECT_NAME} in Release mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O2")
endif ()

add_subdirectory(simulator)
add_subdirectory(statistics)
add_subdirectory(wow_library)
add_subdirectory(local_build)

if (EMSCRIPTEN)
    add_subdirectory(interface)
endif ()
