cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(interface)

add_executable(
        interface
        src/entry_point.cpp
)

target_link_libraries(interface simulator)
set_target_properties(interface PROPERTIES LINK_FLAGS "-s ERROR_ON_UNDEFINED_SYMBOLS=0 -s DEMANGLE_SUPPORT=1 -s TOTAL_MEMORY=640MB -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 --bind")