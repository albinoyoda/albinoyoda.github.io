set(CMAKE_CXX_STANDARD 14)
project(local_build)

include_directories(include)

add_executable(
        local_build
        source/main_web_code.cpp
)

target_include_directories(local_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(local_build wow_library simulator)


