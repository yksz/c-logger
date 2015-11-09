#include "logger.h"
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Logger type */
static const int kConsoleLogger = 0;
static const int kFileLogger = 1;

/* Base logger */
static int s_logger = 0; /* kConsoleLogger */
static LogLevel s_logLevel = LogLevel_INFO;

/* Console logger */
static FILE* s_cl_stream;

/* File logger */
static const char* s_fl_filename;
static FILE* s_fl_fp;
static int s_fl_maxFileSize = 1048576; /* 1 MB */
static unsigned char s_fl_maxBackupFiles;
static int s_fl_currentFileSize;

int logger_useConsoleLogger(FILE* fp)
{
    s_cl_stream = fp;
    s_logger = kConsoleLogger;
    return 1;
}

static long getFileSize(const char* filename)
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

int logger_useFileLogger(const char* filename, int maxFileSize, unsigned char maxBackupFiles)
{
    s_fl_filename = filename;
    if (maxFileSize > 0) {
        s_fl_maxFileSize = maxFileSize;
    }
    s_fl_maxBackupFiles = maxBackupFiles;
    s_fl_currentFileSize = getFileSize(filename);
    s_fl_fp = fopen(filename, "a");
    if (s_fl_fp == NULL) {
        fprintf(stderr, "ERROR: Failed to open file: %s\n", filename);
        return 0;
    }
    s_logger = kFileLogger;
    return 1;
}

void logger_setLogLevel(LogLevel level)
{
    s_logLevel = level;
}

static char* getBackupFileName(const char* basename, unsigned char index)
{
    int len = strlen(basename) + 5; /* <basename>.255\0 */
    char* backupname = (char*) malloc(sizeof(char) * len);
    if (backupname == NULL) {
        fprintf(stderr, "ERROR: Out of memory\n");
        return NULL;
    }
    if (index == 0) {
        sprintf(backupname, "%s", basename);
    } else {
        sprintf(backupname, "%s.%d", basename, index);
    }
    return backupname;
}

static int isFileExists(const char* filename)
{
    FILE* fp;

    if ((fp = fopen(filename, "r")) == NULL) {
        return 0;
    } else {
        fclose(fp);
        return 1;
    }
}

static int rotateLogFiles()
{
    unsigned char i;
    char *src, *dst;

    if (s_fl_currentFileSize < s_fl_maxFileSize) {
        return s_fl_fp != NULL;
    }

    fclose(s_fl_fp);
    for (i = s_fl_maxBackupFiles; i > 0; i--) {
        src = getBackupFileName(s_fl_filename, i - 1);
        dst = getBackupFileName(s_fl_filename, i);
        if (src != NULL && dst != NULL) {
            if (isFileExists(dst)) {
                if (remove(dst) != 0) {
                    fprintf(stderr, "ERROR: Failed to remove file: %s\n", dst);
                }
            }
            if (isFileExists(src)) {
                if (rename(src, dst) != 0) {
                    fprintf(stderr, "ERROR: Failed to rename file: %s -> %s\n", src, dst);
                }
            }
        }
        free(src);
        free(dst);
    }
    s_fl_currentFileSize = getFileSize(s_fl_filename);
    s_fl_fp = fopen(s_fl_filename, "a");
    if (s_fl_fp == NULL) {
        fprintf(stderr, "ERROR: Failed to open file: %s\n", s_fl_filename);
        return 0;
    }
    return 1;
}

static int vflog(LogLevel level, FILE* fp, const char* file, int line, const char* func, const char* fmt, va_list arg)
{
    time_t now;
    char timestr[20];
    const char* levelstr;
    int size;
    int totalsize = 0;

    now = time(NULL);
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %T", localtime(&now));
    switch (level) {
        case LogLevel_DEBUG:
            levelstr = "DEBUG";
            break;
        case LogLevel_INFO:
            levelstr = "INFO ";
            break;
        case LogLevel_WARN:
            levelstr = "WARN ";
            break;
        case LogLevel_ERROR:
            levelstr = "ERROR ";
            break;
        default:
            assert(0 && "Unknown LogLevel");
            return 0;
    }
    if ((size = fprintf(fp, "%s %s %s:%d:%s: ", timestr, levelstr, file, line, func)) > 0) {
        totalsize += size;
    }
    if ((size = vfprintf(fp, fmt, arg)) > 0) {
        totalsize += size;
    }
    if ((size = fprintf(fp, "\n")) > 0) {
        totalsize += size;
    }
    return totalsize;
}

void logger_log(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...)
{
    va_list arg;

    if (s_logLevel > level) {
        return;
    }

    va_start(arg, fmt);
    if (s_logger == kConsoleLogger) {
        s_cl_stream = s_cl_stream != NULL ? s_cl_stream : stdout;
        vflog(level, s_cl_stream, file, line, func, fmt, arg);
    } else if (s_logger == kFileLogger) {
        if (rotateLogFiles()) {
            s_fl_currentFileSize += vflog(level, s_fl_fp, file, line, func, fmt, arg);
        }
    } else {
        assert(0 && "Unknown logger");
    }
    va_end(arg);
}
