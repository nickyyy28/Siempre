
find_package(libLogger)

set(EXECUTABLE_OUTPUT_PATH &{PROJECT_SOURCE_DIR}/bin)

find_library(LIB_LOGGER Logger HINTS ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(testLogger testLogger.cpp)

target_link_libraries(testLogger ${LIB_LOGGER})
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
