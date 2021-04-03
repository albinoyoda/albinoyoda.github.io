set(CMAKE_CXX_STANDARD 14)
project(statistics)

include_directories(include)

add_library(
        statistics
        source/Statistics.cpp
)

target_include_directories(statistics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif ()