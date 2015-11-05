#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
# endif /* __cplusplus */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define LOG_DEBUG(fmt, ...) log_fprintln(LogLevel_DEBUG, stderr, __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LOG_INFO(fmt, ...)  log_fprintln(LogLevel_INFO , stderr, __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LOG_WARN(fmt, ...)  log_fprintln(LogLevel_WARN , stderr, __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_fprintln(LogLevel_ERROR, stderr, __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)

#ifdef _WIN32
#include <string.h>
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __func__ __FUNCTION__
#else
#define __FILENAME__ __FILE__
#endif

typedef enum
{
    LogLevel_DEBUG,
    LogLevel_INFO,
    LogLevel_WARN,
    LogLevel_ERROR,
} LogLevel;

static LogLevel currentLevel = LogLevel_INFO;

void log_setLevel(LogLevel newLevel)
{
    currentLevel = newLevel;
}

void log_vfprintln(LogLevel level, FILE* fp, const char* file, int line, const char* func, const char* fmt, va_list arg)
{
    time_t now;
    char timestr[20];

    if (currentLevel > level) {
        return;
    }

    now = time(NULL);
    strftime(timestr, sizeof(timestr), "%Y/%m/%d %H:%M:%S", localtime(&now));

    switch (level) {
        case LogLevel_DEBUG:
            fprintf(fp, "%s DEBUG %s:%d:%s: ", timestr, file, line, func);
            break;
        case LogLevel_INFO:
            fprintf(fp, "%s INFO  %s:%d:%s: ", timestr, file, line, func);
            break;
        case LogLevel_WARN:
            fprintf(fp, "%s WARN  %s:%d:%s: ", timestr, file, line, func);
            break;
        case LogLevel_ERROR:
            fprintf(fp, "%s ERROR %s:%d:%s: ", timestr, file, line, func);
            break;
        default:
            assert(0 && "Unknown LogLevel");
    }

    vfprintf(fp, fmt, arg);
    fprintf(fp, "\n");
}

void log_fprintln(LogLevel level, FILE* fp, const char* file, int line, const char* func, const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    log_vfprintln(level, fp, file, line, func, fmt, arg);
    va_end(arg);
}

#ifdef __cplusplus
} /* extern "C" */
# endif /* __cplusplus */

#endif /* LOGGER_H */
