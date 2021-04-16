cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project(wasm_interface)

add_executable(
        ${PROJECT_NAME}
        source/emscripten_bindings.cpp
)

target_link_libraries(${PROJECT_NAME} simulator wow_library)

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-O2 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s DEMANGLE_SUPPORT=1 -s TOTAL_MEMORY=640MB -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 --bind")