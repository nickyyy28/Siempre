cmake_minimum_required (VERSION 2.8)
add_definitions(-std=c++17)

project(Siempre)

add_subdirectory(src)
add_subdirectory(lib)

if (EXISTS ${PROJECT_SOURCE_DIR}/test)
    add_subdirectory(test)
endif()

# INSTALL(TARGETS ${PROJECT_SOURCE_DIR}/lib/libsiem.so LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)


install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/siem)
