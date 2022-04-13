
find_package(libLogger)

set(EXECUTABLE_OUTPUT_PATH &{PROJECT_SOURCE_DIR}/bin)

find_library(LIB_LOGGER Logger HINTS ${PROJECT_SOURCE_DIR}/lib)
find_library(LIB_CONFIGURATOR Configurator HINTS ${PROJECT_SOURCE_DIR}/lib)
find_library(LIB_THREAD Thread HINTS ${PROJECT_SOURCE_DIR}/lib)

link_directories(${PROJECT_SOURCE_DIR}/3rd-party/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd-party/include)

set(
    3RD_PARTY
    yaml-cpp
    cityhash
)

add_executable(testLogger testLogger.cpp)
add_executable(testSingleTon testSingleTon.cpp)
add_executable(testLog testLog.cpp ${PROJECT_SOURCE_DIR}/src/Logger/Logger.cc)
add_executable(testYaml testyaml.cpp)
add_executable(testYamlConfig testYamlConfig.cpp)
add_executable(testConfig testConfig.cpp)
add_executable(testFromYaml testFromYaml.cpp)
add_executable(testClassConfig testClassConfig.cpp)
add_executable(testConfigChange testConfigChange.cpp)
add_executable(testLogConfig testLogConfig.cpp)
add_executable(testLoadLogFromYaml testLoadLogFromYaml.cpp)
add_executable(testThread testThread.cpp)

target_link_libraries(testThread ${LIB_LOGGER} ${LIB_THREAD})
target_link_libraries(testLoadLogFromYaml ${LIB_LOGGER} ${LIB_CONFIGURATOR} ${3RD_PARTY})
target_link_libraries(testLogConfig ${LIB_LOGGER} ${LIB_CONFIGURATOR} ${3RD_PARTY})
target_link_libraries(testConfigChange ${LIB_LOGGER} ${LIB_CONFIGURATOR} ${3RD_PARTY})
target_link_libraries(testClassConfig ${LIB_LOGGER} ${LIB_CONFIGURATOR} ${3RD_PARTY})
target_link_libraries(testFromYaml ${LIB_LOGGER} ${LIB_CONFIGURATOR} ${3RD_PARTY})
target_link_libraries(testYamlConfig ${LIB_LOGGER} ${LIB_CONFIGURATOR} ${3RD_PARTY})
target_link_libraries(testYaml yaml-cpp)
target_link_libraries(testConfig ${LIB_LOGGER} ${LIB_CONFIGURATOR} yaml-cpp)
target_link_libraries(testLog ${LIB_LOGGER})
target_link_libraries(testLogger ${LIB_LOGGER})


set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
