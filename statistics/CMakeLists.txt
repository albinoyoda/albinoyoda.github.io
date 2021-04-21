set(CMAKE_CXX_STANDARD 17)
project(statistics)

add_library(
        ${PROJECT_NAME}
        source/Statistics.cpp
        source/Distribution.cpp
        source/BinomialDistribution.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif ()