cmake_minimum_required (VERSION 2.8)
add_definitions(-std=c++17)

project(Siempre)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)

add_subdirectory(src)
add_subdirectory(lib)

if (EXISTS ${PROJECT_SOURCE_DIR}/test)
    add_subdirectory(test)
endif()

