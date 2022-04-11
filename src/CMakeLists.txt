include_directories(${PROJECT_SOURCE_DIR}/include)

set(
    LOGGER_LIST
    ./Logger/Logger.cc
)

set(
    CONFIGURATOR_LIST
    ./Configurator/Configurator.cc
)

include_directories(${PROJECT_SOURCE_DIR}/3rd-party/include)

link_directories(${PROJECT_SOURCE_DIR}/3rd-party/lib)

add_definitions(-fPIC)
add_library(Logger_shared SHARED ${LOGGER_LIST})
add_library(Logger_static STATIC ${LOGGER_LIST})

add_library(Configurator_shared SHARED ${CONFIGURATOR_LIST})
add_library(Configurator_static STATIC ${CONFIGURATOR_LIST})

# target_link_libraries(Configurator_shared yaml-cpp)
# target_link_libraries(Configurator_static yaml-cpp)

set_target_properties(Logger_shared PROPERTIES OUTPUT_NAME "Logger")
set_target_properties(Logger_static PROPERTIES OUTPUT_NAME "Logger")

set_target_properties(Configurator_shared PROPERTIES OUTPUT_NAME "Configurator")
set_target_properties(Configurator_static PROPERTIES OUTPUT_NAME "Configurator")

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
