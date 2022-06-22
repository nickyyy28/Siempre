set(EXECUTABLE_OUTPUT_PATH &{PROJECT_SOURCE_DIR}/bin)

find_library(LIB_SIEM siem HINTS ${PROJECT_SOURCE_DIR}/lib)

set(
    3RD_PARTY
    yaml-cpp
    cityhash
)

set(
    SIEM_LIBRAARIES
    ${LIB_SIEM}
    ${3RD_PARTY}
    -pthread
)

add_definitions("-Wall -g")

link_directories(${PROJECT_SOURCE_DIR}/3rd-party/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd-party/include)

add_executable(testScheduler testScheduler.cpp)
add_executable(testEnv testEnv.cpp)
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
add_executable(testMutithreadLogger testMutithreadLogger.cpp)
add_executable(testMacro testMacro.cpp)
add_executable(testBackTrace testBackTrace.cpp)
add_executable(testAssert testAssert.cpp)
add_executable(testFiber testFiber.cpp)
add_executable(testThreadPool testThreadPool.cpp)
add_executable(testSString ./testSString.cpp)

target_link_libraries(testSString ${SIEM_LIBRAARIES})
target_link_libraries(testScheduler ${SIEM_LIBRAARIES})
target_link_libraries(testFiber ${SIEM_LIBRAARIES})
target_link_libraries(testAssert ${SIEM_LIBRAARIES})
target_link_libraries(testBackTrace ${SIEM_LIBRAARIES})
target_link_libraries(testMacro ${SIEM_LIBRAARIES})
target_link_libraries(testEnv ${SIEM_LIBRAARIES})
target_link_libraries(testMutithreadLogger ${SIEM_LIBRAARIES})
target_link_libraries(testThread ${SIEM_LIBRAARIES})
target_link_libraries(testLoadLogFromYaml ${SIEM_LIBRAARIES})
target_link_libraries(testLogConfig ${SIEM_LIBRAARIES})
target_link_libraries(testConfigChange ${SIEM_LIBRAARIES})
target_link_libraries(testClassConfig ${SIEM_LIBRAARIES})
target_link_libraries(testFromYaml ${SIEM_LIBRAARIES})
target_link_libraries(testYamlConfig ${SIEM_LIBRAARIES})
target_link_libraries(testYaml ${SIEM_LIBRAARIES})
target_link_libraries(testConfig ${SIEM_LIBRAARIES})
target_link_libraries(testLog ${SIEM_LIBRAARIES})
target_link_libraries(testLogger ${SIEM_LIBRAARIES})
target_link_libraries(testThreadPool ${SIEM_LIBRAARIES})


set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
