#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
# endif /* __cplusplus */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __func__ __FUNCTION__
#else
#define __FILENAME__ __FILE__
#endif /* _WIN32 */

#define LOG_DEBUG(fmt, ...) log_write(LogLevel_DEBUG, __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LOG_INFO(fmt, ...)  log_write(LogLevel_INFO , __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LOG_WARN(fmt, ...)  log_write(LogLevel_WARN , __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_write(LogLevel_ERROR, __FILENAME__, __LINE__, __func__, fmt, ## __VA_ARGS__)

typedef enum
{
    LogLevel_DEBUG,
    LogLevel_INFO,
    LogLevel_WARN,
    LogLevel_ERROR,
} LogLevel;

static LogLevel currentlevel = LogLevel_INFO;
static const char* logfilename = "";
static int maxFileSize = 1048576; /* 1MB */
static int maxBackupFiles = 5;
static int currentFileSize;
static FILE* logfile;

static int file_exists(const char* filename)
{
    FILE* fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        return 0;
    } else {
        fclose(fp);
        return 1;
    }
}

static long file_size(const char* filename)
{
    FILE* fp;
    long size;

    if ((fp = fopen(filename, "rb")) == NULL) {
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}

static char* file_backupname(const char* basename, int index)
{
    assert(index >= 0 && index <= 999);

    int len = strlen(basename);
    len += 5; /* <basename>.999\0 */
    char* backupname = (char*) malloc(sizeof(char) * len);
    if (backupname == NULL) {
        return NULL;
    }
    if (index == 0) {
        sprintf(backupname, "%s", basename);
    } else {
        sprintf(backupname, "%s.%d", basename, index);
    }
    return backupname;
}


void log_init(LogLevel level, const char* filename)
{
    currentlevel = level;
    logfilename = filename;
    currentFileSize = file_size(logfilename);
    if ((logfile = fopen(logfilename, "a")) == NULL) {
        return;
    }
}

void log_vfprintln(LogLevel level, FILE* fp, const char* file, int line, const char* func, const char* fmt, va_list arg)
{
    time_t now;
    char timestr[20];
    int size = 0;

    now = time(NULL);
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&now));
    switch (level) {
        case LogLevel_DEBUG:
            size = fprintf(fp, "%s DEBUG %s:%d:%s: ", timestr, file, line, func);
            break;
        case LogLevel_INFO:
            size = fprintf(fp, "%s INFO  %s:%d:%s: ", timestr, file, line, func);
            break;
        case LogLevel_WARN:
            size = fprintf(fp, "%s WARN  %s:%d:%s: ", timestr, file, line, func);
            break;
        case LogLevel_ERROR:
            size = fprintf(fp, "%s ERROR %s:%d:%s: ", timestr, file, line, func);
            break;
        default:
            assert(0 && "Unknown LogLevel");
    }
    if (size > 0) {
        currentFileSize += size;
    }
    vfprintf(fp, fmt, arg);
    fprintf(fp, "\n");
}

static void log_rotate()
{
    int i;
    char* src;
    char* dst;

    if (currentFileSize < maxFileSize) {
        return;
    }
    for (i = maxBackupFiles; i > 0; i--) {
        src = file_backupname(logfilename, i - 1);
        dst = file_backupname(logfilename, i);
        if (src == NULL || dst == NULL) {
            free(src);
            free(dst);
            continue;
        }
        remove(dst);
        rename(src, dst);
        free(src);
        free(dst);
    }
    currentFileSize = file_size(logfilename);
    fclose(logfile);
    if ((logfile = fopen(logfilename, "a")) == NULL) {
        return;
    }
}

void log_write(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...)
{
    va_list arg;

    if (currentlevel > level) {
        return;
    }
    log_rotate();
    va_start(arg, fmt);
    log_vfprintln(level, logfile, file, line, func, fmt, arg);
    va_end(arg);
}

#ifdef __cplusplus
} /* extern "C" */
# endif /* __cplusplus */

#endif /* LOGGER_H */

