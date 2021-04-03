cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(wow_simulator)

include(FetchContent)
#FetchContent_Declare(
#        googletest
#        GIT_REPOSITORY https://github.com/google/googletest.git
#        GIT_TAG        release-1.8.0
#)
FetchContent_Declare(
        emscripten
        GIT_REPOSITORY https://github.com/emscripten-core/emscripten.git
        GIT_TAG        main
)
FetchContent_MakeAvailable(emscripten)


include(CTest)
enable_testing()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building ${CMAKE_PROJECT_NAME} in debug mode")
    add_compile_options(-Wall -Wextra -pedantic -Werror)
else ()
    add_compile_options(-O2)
endif ()

add_subdirectory(simulator)
add_subdirectory(statistics)
add_subdirectory(wow_library)

#if (EMSCRIPTEN)
    add_subdirectory(interface)
#else ()
    add_subdirectory(google_tests/lib)
    add_subdirectory(local_build)
#endif ()

