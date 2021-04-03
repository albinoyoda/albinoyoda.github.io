set(CMAKE_CXX_STANDARD 17)
project(wow_library)

add_library(
        ${PROJECT_NAME}
        source/Item.cpp
        source/Character.cpp
        source/Attributes.cpp
        source/Armory.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} common)

if (NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif ()