set(CMAKE_CXX_STANDARD 17)
project(local_build)

include_directories(include)

add_executable(
        ${PROJECT_NAME}
        main_web_code.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} wow_library simulator)


