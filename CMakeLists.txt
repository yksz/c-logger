cmake_minimum_required(VERSION 2.8.7)
project(logger C)

set(CMAKE_C_FLAGS "-Wall -std=c89")
set(CMAKE_C_FLAGS_DEBUG "-g")

### Library
set(source_files
    src/logger.c
)
include_directories(
    ${PROJECT_SOURCE_DIR}/src
)
# shared and static libraries
add_library(${PROJECT_NAME} SHARED ${source_files})
add_library(${PROJECT_NAME}_static STATIC ${source_files})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_static PROPERTIES PREFIX "lib")

### Document
add_custom_target(doc COMMAND "doxygen" "${PROJECT_SOURCE_DIR}/doc/Doxyfile")
