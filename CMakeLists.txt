cmake_minimum_required(VERSION 2.8.7)
project(logger C)

set(CMAKE_C_FLAGS "-Wall -std=c89")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-g")

option(build_tests "Build all of own tests" OFF)
option(build_examples "Build example programs" OFF)
option(build_docs "Build doxygen documentation" OFF)

### Library
set(source_files
    src/logger.c
    src/loggerconf.c
)
include_directories(
    ${PROJECT_SOURCE_DIR}/src
)
# shared and static libraries
add_library(${PROJECT_NAME} SHARED ${source_files})
add_library(${PROJECT_NAME}_static STATIC ${source_files})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

### Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
file(GLOB header_files src/*.h)
install(FILES ${header_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

### Document
if(build_docs)
    add_custom_target(doc COMMAND "doxygen" "${PROJECT_SOURCE_DIR}/doc/Doxyfile")
endif()

### Test
if(build_tests)
    enable_testing()
    add_subdirectory(test)
endif()

### Example
if(build_examples)
    add_subdirectory(example)
endif()
