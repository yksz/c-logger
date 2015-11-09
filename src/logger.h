#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
# endif /* __cplusplus */

#include <stdio.h>

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

int logger_initAsConsoleLogger(FILE* fp);
int logger_initAsFileLogger(const char* filename, int maxFileSize, unsigned char maxBackupFiles);
void logger_setLogLevel(LogLevel level);
void logger_log(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...);

#ifdef __cplusplus
} /* extern "C" */
# endif /* __cplusplus */

#endif /* LOGGER_H */
