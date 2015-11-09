#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
# endif /* __cplusplus */

#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
 #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
 #define __func__ __FUNCTION__
#else
 #define __FILENAME__ __FILE__
#endif /* _WIN32 || _WIN64 */

#define LOG_DEBUG(fmt, ...) logger_log(LogLevel_DEBUG, __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LOG_INFO(fmt, ...)  logger_log(LogLevel_INFO , __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LOG_WARN(fmt, ...)  logger_log(LogLevel_WARN , __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LOG_ERROR(fmt, ...) logger_log(LogLevel_ERROR, __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)

typedef enum
{
    LogLevel_DEBUG,
    LogLevel_INFO,
    LogLevel_WARN,
    LogLevel_ERROR,
} LogLevel;

/**
 * Initialize the logger as a console logger.
 * Before starting a logging, make sure call this function.
 * If the file pointer is NULL, stdout will be used.
 *
 * @param[in] fp A file pointer. Set stdout or stderr.
 * @return Non-zero value upon success or 0 on error
 * @see logger_initAsFileLogger()
 */
int logger_initAsConsoleLogger(FILE* fp);

/**
 * Initialize the logger as a file logger.
 * Before starting a logging, make sure call this function.
 *
 * @param[in] filename The name of the output file
 * @param[in] maxFileSize The maximum number of bytes to write to any one file
 * @param[in] maxBackupFiles The maximum number of files for backup
 * @return Non-zero value upon success or 0 on error
 * @see logger_initAsConsoleLogger()
 */
int logger_initAsFileLogger(const char* filename, int maxFileSize, unsigned char maxBackupFiles);

/**
 * @param[in] level A new log level
 */
void logger_setLogLevel(LogLevel level);

/**
 * @param[in] level A log level
 * @param[in] file A file name string
 * @param[in] line A line number
 * @param[in] func A function name string
 * @param[in] fmt A format string
 * @param[in] ... Additional arguments
 */
void logger_log(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...);

#ifdef __cplusplus
} /* extern "C" */
# endif /* __cplusplus */

#endif /* LOGGER_H */
