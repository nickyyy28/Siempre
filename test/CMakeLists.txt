
find_package(libLogger)

set(EXECUTABLE_OUTPUT_PATH &{PROJECT_SOURCE_DIR}/bin)

find_library(LIB_LOGGER Logger HINTS ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(testLogger testLogger.cpp)
add_executable(testSingleTon testSingleTon.cpp)
add_executable(testLog testLog.cpp ${PROJECT_SOURCE_DIR}/src/Logger/Logger.cc)

target_link_libraries(testLog ${LIB_LOGGER})
target_link_libraries(testLogger ${LIB_LOGGER})
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
