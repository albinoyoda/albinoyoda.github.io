set(CMAKE_CXX_STANDARD 17)
project(common)

add_library(${PROJECT_NAME}
        source/string_helpers.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME})

if (NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif ()