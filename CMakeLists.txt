cmake_minimum_required (VERSION 2.8)
add_definitions(-std=c++14)

# set(CMAKE_C_COMPILER "/home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc")
# set(CMAKE_CXX_COMPILER "/home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++")

project(Siempre)

add_subdirectory(src)
add_subdirectory(test)
