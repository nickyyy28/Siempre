include_directories(${PROJECT_SOURCE_DIR}/include)

set(
    SIEM_LIST
    ./Configurator/Configurator.cc
    ./Thread/Thread.cc
    ./Logger/Logger.cc
    ./utils/utils.cc
    ./Thread/Mutex.cc
)

include_directories(${PROJECT_SOURCE_DIR}/3rd-party/include)

link_directories(${PROJECT_SOURCE_DIR}/3rd-party/lib)

add_definitions(-fPIC)
add_library(siem_shared SHARED ${SIEM_LIST})
add_library(siem_static STATIC ${SIEM_LIST})

set_target_properties(siem_shared PROPERTIES OUTPUT_NAME "siem")
set_target_properties(siem_static PROPERTIES OUTPUT_NAME "siem")

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
