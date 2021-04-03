set(CMAKE_CXX_STANDARD 14)
project(wow_library)

include_directories(include)

add_library(
        wow_library
        source/Item.cpp
        source/Character.cpp
        source/Attributes.cpp
        source/Armory.cpp
)

target_include_directories(wow_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(wow_library)

if (NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif ()