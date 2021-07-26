include_directories(${PROJECT_SOURCE_DIR}/include)

set(
    LOGGER_LIST
    ./Logger/Logger.cc
)

add_library(Logger_shared SHARED ${LOGGER_LIST})
add_library(Logger_static STATIC ${LOGGER_LIST})

set_target_properties(Logger_shared PROPERTIES OUTPUT_NAME "Logger")
set_target_properties(Logger_static PROPERTIES OUTPUT_NAME "Logger")

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
