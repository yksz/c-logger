set(examples
    logger_console_example
    logger_file_example
    logger_multi_example
    loggerconf_example
)
include_directories(
    ${PROJECT_SOURCE_DIR}/src
)
foreach(example IN LISTS examples)
    add_executable(${example} ${example}.c)
    target_link_libraries(${example} ${PROJECT_NAME}_static)
endforeach()
