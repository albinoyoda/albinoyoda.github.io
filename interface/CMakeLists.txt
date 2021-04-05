cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(wow_interface)

add_executable(
        ${PROJECT_NAME}
        src/entry_point.cpp
)

target_link_libraries(${PROJECT_NAME} simulator)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s ERROR_ON_UNDEFINED_SYMBOLS=0 -s DEMANGLE_SUPPORT=1 -s TOTAL_MEMORY=640MB -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 --bind")