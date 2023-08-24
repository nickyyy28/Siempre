include_directories(${PROJECT_SOURCE_DIR}/include)

set(
    SIEM_LIST
    ./Configurator/Configurator.cc
    ./Configurator/Env.cc
    ./Thread/Thread.cc
    ./Logger/Logger.cc
    ./utils/utils.cc
    ./Thread/Mutex.cc
    ./Fiber/Fiber.cc
    ./Fiber/Scheduler.cc
    ./Fiber/SimpleScheduler.cc
    ./io/IOManager.cc
    ./fs/fs.cc
    ./Thread/ThreadPool.cc
    ./Thread/Timer.cc
    ./base/SString.cc
    ./base/TimeStamp.cc
    ./net/InetAddress.cc
    ./net/Channel.cc
    ./net/Poller.cc
    ./net/EventLoop.cc
    ./net/TCPClient.cc
    ./net/TCPConn.cc
    ./net/TCPServer.cc
    ./base/Console.cc
)

include_directories(${PROJECT_SOURCE_DIR}/3rd-party/include)

link_directories(${PROJECT_SOURCE_DIR}/3rd-party/lib)

add_definitions(-fPIC)
add_library(siem_shared SHARED ${SIEM_LIST})
add_library(siem_static STATIC ${SIEM_LIST})

target_include_directories(siem_shared PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_include_directories(siem_static PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

set_target_properties(siem_shared PROPERTIES 
    OUTPUT_NAME "siem")
set_target_properties(siem_static PROPERTIES 
    OUTPUT_NAME "siem")


install(TARGETS siem_shared siem_static
    EXPORT siem-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/siem/)

install(EXPORT siem-targets
    NAMESPACE libsiem::
    # INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/siem
    FILE libsiem-config.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/libsiem)

add_dependencies(siem_shared siem_static build_yamlcpp)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
