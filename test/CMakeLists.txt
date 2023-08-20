set(EXECUTABLE_OUTPUT_PATH &{PROJECT_SOURCE_DIR}/bin)

# find_library(LIB_SIEM siem HINTS ${PROJECT_SOURCE_DIR}/lib)

SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_INSTALL_PREFIX})


find_package(libsiem REQUIRED)

if(libsiem_FOUND)
    message("libsiem found...")
    include_directories(${libsiem_INCLUDE_DIR})
else(libsiem_FOUND)
    message(FATAL_ERROR "libsiem not found")
endif(libsiem_FOUND)
 
set(
    3RD_PARTY
    yaml-cpp
    cityhash
)

# set(
#     SIEM_LIBRARIES
#     ${LIB_SIEM}
#     ${3RD_PARTY}
#     pthread
# )


set(
    SIEM_LIBRARIES
    libsiem::siem_shared
    ${3RD_PARTY}
    pthread
)

add_definitions("-g -O0")

link_directories(${PROJECT_SOURCE_DIR}/3rd-party/lib)

# include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd-party/include)

add_executable(testSimpleScheduler testSimpleScheduler.cpp)
add_executable(testColorConsole testColorConsole.cpp)
add_executable(testWeakPtr testWeakPtr.cpp)
add_executable(testSharedPtr testSharedPtr.cpp)
add_executable(testScheduler testScheduler.cpp)
add_executable(testEnv testEnv.cpp)
add_executable(testLogger testLogger.cpp)
add_executable(testSingleTon testSingleTon.cpp)
add_executable(testLog testLog.cpp)
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
add_executable(testCrossRef testCrossRef.cpp)
add_executable(testBind testBind.cpp)
add_executable(testFunctional testFunctional.cpp)
add_executable(testSStringList testSStringList.cpp)
add_executable(testReplaceAndInsert testReplaceAndInsert.cpp)
add_executable(testSemaphore testSemaphore.cpp)
add_executable(testTimer testTimer.cpp)
add_executable(testTimeStamp testTimeStamp.cpp)
add_executable(testDecltype testDecltype.cpp)
add_executable(testFS testFS.cpp)
add_executable(testPlacement testPlacement.cpp)

target_link_libraries(testFS ${SIEM_LIBRARIES})
target_link_libraries(testSingleTon ${SIEM_LIBRARIES})
target_link_libraries(testSimpleScheduler ${SIEM_LIBRARIES})
target_link_libraries(testColorConsole ${SIEM_LIBRARIES})
target_link_libraries(testSemaphore ${SIEM_LIBRARIES})
target_link_libraries(testTimer ${SIEM_LIBRARIES})
target_link_libraries(testDecltype ${SIEM_LIBRARIES})
target_link_libraries(testTimeStamp ${SIEM_LIBRARIES})
target_link_libraries(testReplaceAndInsert ${SIEM_LIBRARIES})
target_link_libraries(testSStringList ${SIEM_LIBRARIES})
target_link_libraries(testFunctional ${SIEM_LIBRARIES})
target_link_libraries(testCrossRef ${SIEM_LIBRARIES})
target_link_libraries(testWeakPtr ${SIEM_LIBRARIES})
target_link_libraries(testBind ${SIEM_LIBRARIES})
target_link_libraries(testSharedPtr ${SIEM_LIBRARIES})
target_link_libraries(testSString ${SIEM_LIBRARIES})
target_link_libraries(testScheduler ${SIEM_LIBRARIES})
target_link_libraries(testFiber ${SIEM_LIBRARIES})
target_link_libraries(testAssert ${SIEM_LIBRARIES})
target_link_libraries(testBackTrace ${SIEM_LIBRARIES})
target_link_libraries(testMacro ${SIEM_LIBRARIES})
target_link_libraries(testEnv ${SIEM_LIBRARIES})
target_link_libraries(testMutithreadLogger ${SIEM_LIBRARIES})
target_link_libraries(testThread ${SIEM_LIBRARIES})
target_link_libraries(testLoadLogFromYaml ${SIEM_LIBRARIES})
target_link_libraries(testLogConfig ${SIEM_LIBRARIES})
target_link_libraries(testConfigChange ${SIEM_LIBRARIES})
target_link_libraries(testClassConfig ${SIEM_LIBRARIES})
target_link_libraries(testFromYaml ${SIEM_LIBRARIES})
target_link_libraries(testYamlConfig ${SIEM_LIBRARIES})
target_link_libraries(testYaml ${SIEM_LIBRARIES})
target_link_libraries(testConfig ${SIEM_LIBRARIES})
target_link_libraries(testLog ${SIEM_LIBRARIES})
target_link_libraries(testLogger ${SIEM_LIBRARIES})
target_link_libraries(testThreadPool ${SIEM_LIBRARIES})
target_link_libraries(testLog ${SIEM_LIBRARIES})


set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
