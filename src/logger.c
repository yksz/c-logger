#include "logger.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static long getFileSize(const char* filename);
static int vflog(LogLevel level, FILE* fp, const char* file, int line, const char* func, const char* fmt, va_list arg);
static void rotateLogFiles();
static char* getBackupFileName(const char* basename, int index);
static int isFileExists(const char* filename);

static LogLevel s_logLevel = LogLevel_INFO;
static LogStream s_logStream = LogStream_STDOUT;
static const char* s_logFileName = "";
static int s_maxFileSize = 1048576; /* 1MB */
static int s_maxBackupFiles = 5;
static int s_currentFileSize;
static FILE* s_logFile;

int logger_init(LogLevel level, const char* filename)
{
    s_logLevel = level;
    s_logFileName = filename;
    s_currentFileSize = getFileSize(filename);
    s_logFile = fopen(filename, "a");
    s_logStream = LogStream_FILE;
    if (s_logFile == NULL) {
        return 0;
    }
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

void logger_log(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
    va_list arg;

    if (s_logLevel > level) {
        return;
    }

    va_start(arg, fmt);
    switch (s_logStream) {
        case LogStream_STDOUT:
            vflog(level, stdout, file, line, func, fmt, arg);
            break;

        case LogStream_STDERR:
            vflog(level, stderr, file, line, func, fmt, arg);
            break;

        case LogStream_FILE:
            rotateLogFiles();
            s_currentFileSize += vflog(level, s_logFile, file, line, func, fmt, arg);
            break;

        default:
            assert(0 && "Unknown LogStream");
            break;
    }
    va_end(arg);
}

static int vflog(LogLevel level, FILE* fp, const char* file, int line, const char* func, const char* fmt, va_list arg)
{
    time_t now;
    char timestr[20];
    char* levelstr;
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

static void rotateLogFiles()
{
    int i;
    char* src;
    char* dst;

    if (s_currentFileSize < s_maxFileSize) {
        return;
    }
    for (i = s_maxBackupFiles; i > 0; i--) {
        src = getBackupFileName(s_logFileName, i - 1);
        dst = getBackupFileName(s_logFileName, i);
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
    s_currentFileSize = getFileSize(s_logFileName);
    fclose(s_logFile);
    if ((s_logFile = fopen(s_logFileName, "a")) == NULL) {
        return;
    }
}

static char* getBackupFileName(const char* basename, int index)
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
