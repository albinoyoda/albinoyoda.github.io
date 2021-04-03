set(CMAKE_CXX_STANDARD 17)
project(local_build)

include_directories(include)

add_executable(
        ${PROJECT_NAME}_simulator
        main_web_code.cpp
)

target_include_directories(${PROJECT_NAME}_simulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}_simulator wow_library simulator)

add_executable(
        ${PROJECT_NAME}_multi_select
        main_multi_select.cpp
)

target_include_directories(${PROJECT_NAME}_multi_select PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}_multi_select wow_library simulator)


