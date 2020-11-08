cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(wow_simulator)

include_directories(wow_library/include/)

if (EMSCRIPTEN)

    add_executable(
            wow_interface
            interface/src/entry_point.cpp
    )

    target_link_libraries(wow_interface wow_library)
    set_target_properties(wow_interface PROPERTIES LINK_FLAGS "-s ERROR_ON_UNDEFINED_SYMBOLS=0 -s DEMANGLE_SUPPORT=1 -s TOTAL_MEMORY=640MB -s ASSERTIONS=1 --bind")

else ()

    # Main executable when running offline
    add_executable(wow_web main_web_code.cpp)
    target_link_libraries(wow_web wow_library)

    add_subdirectory(Google_tests)

endif ()

add_subdirectory(wow_library)