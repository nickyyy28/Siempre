cmake_minimum_required (VERSION 2.8)
add_definitions(-std=c++17)

project(Siempre)

add_subdirectory(src)

if (EXISTS ${PROJECT_SOURCE_DIR}/test)
    add_subdirectory(test)
endif()


