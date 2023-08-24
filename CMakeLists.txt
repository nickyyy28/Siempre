cmake_minimum_required (VERSION 2.8)
add_definitions(-std=c++17)

project(Siempre)

# set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)

add_custom_target(build_yamlcpp
    COMMAND cd ${PROJECT_SOURCE_DIR}/github/yaml-cpp && mkdir -p build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/3rd-party && make && make install
    COMMENT "Building yaml-cpp"
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/3rd-party/include/yaml-cpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/3rd-party/lib/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)

add_subdirectory(src)

if (${ENABLE_TEST})
    message("Enable Test Code")
    add_subdirectory(test)

else()
endif()


