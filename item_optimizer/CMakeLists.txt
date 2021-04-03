set(CMAKE_CXX_STANDARD 17)
project(item_optimizer)

add_library(${PROJECT_NAME}
        source/item_heuristics.cpp
        source/Item_optimizer.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} wow_library common simulator)

if (NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif ()