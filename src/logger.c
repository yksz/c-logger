#include "logger.h"
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#if defined(_WIN32) || defined(_WIN64)
 #include <winsock2.h>
#else
 #include <pthread.h>
 #include <sys/time.h>
#endif /* defined(_WIN32) || defined(_WIN64) */

/* Logger type */
static const int kConsoleLogger = 0;
static const int kFileLogger = 1;

/* Base logger */
static int s_logger;
static enum LogLevel s_logLevel = LogLevel_INFO;
static int s_initialized = 0; /* false */
#if defined(_WIN32) || defined(_WIN64)
static CRITICAL_SECTION s_mutex;
#else
static pthread_mutex_t s_mutex;
#endif /* defined(_WIN32) || defined(_WIN64) */

/* Console logger */
static FILE* s_cl_stream;

/* File logger */
static const char* s_fl_filename;
static FILE* s_fl_fp;
static long s_fl_maxFileSize = 1048576L; /* 1 MB */
static unsigned char s_fl_maxBackupFiles;
static long s_fl_currentFileSize;

static void init(void)
{
    if (s_initialized) {
        return;
    }
#if defined(_WIN32) || defined(_WIN64)
    InitializeCriticalSection(&s_mutex);
#else
    pthread_mutex_init(&s_mutex, NULL);
#endif /* defined(_WIN32) || defined(_WIN64) */
    s_initialized = 1; /* true */
}

static void lock(void)
{
#if defined(_WIN32) || defined(_WIN64)
    EnterCriticalSection(&s_mutex);
#else
    pthread_mutex_lock(&s_mutex);
#endif /* defined(_WIN32) || defined(_WIN64) */
}

static void unlock(void)
{
#if defined(_WIN32) || defined(_WIN64)
    LeaveCriticalSection(&s_mutex);
#else
    pthread_mutex_unlock(&s_mutex);
#endif /* defined(_WIN32) || defined(_WIN64) */
}

#if defined(_WIN32) || defined(_WIN64)
static int gettimeofday(struct timeval* tv, void* tz)
{
    const UINT64 epochFileTime = 116444736000000000ULL;
    FILETIME ft;
    ULARGE_INTEGER li;
    UINT64 t;

    if (tv == NULL) {
        return -1;
    }
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    t = (li.QuadPart - epochFileTime) / 10;
    tv->tv_sec = t / 1000000;
    tv->tv_usec = t % 1000000;
    return 0;
}
#endif /* defined(_WIN32) || defined(_WIN64) */

int logger_initConsoleLogger(FILE* fp)
{
    fp = (fp != NULL) ? fp : stdout;
    if (fp != stdout && fp != stderr) {
        assert(0 && "fp must be stdout or stderr");
        return 0;
    }

    s_cl_stream = fp;
    s_logger = kConsoleLogger;
    init();
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

int logger_initFileLogger(const char* filename, long maxFileSize, unsigned char maxBackupFiles)
{
    if (filename == NULL) {
        assert(0 && "filename must not be null");
        return 0;
    }

    if (s_fl_fp != NULL) { /* reinit */
        fclose(s_fl_fp);
    }
    s_fl_fp = fopen(filename, "a");
    if (s_fl_fp == NULL) {
        fprintf(stderr, "ERROR: logger: Failed to open file: %s\n", filename);
        return 0;
    }
    s_fl_currentFileSize = getFileSize(filename);
    s_fl_filename = filename;
    if (maxFileSize > 0) {
        s_fl_maxFileSize = maxFileSize;
    }
    s_fl_maxBackupFiles = maxBackupFiles;
    s_logger = kFileLogger;
    init();
    return 1;
}

void logger_setLevel(enum LogLevel level)
{
    s_logLevel = level;
}

static char* getBackupFileName(const char* basename, unsigned char index)
{
    int len = strlen(basename) + 5; /* <basename>.255\0 */
    char* backupname = (char*) malloc(sizeof(char) * len);
    if (backupname == NULL) {
        fprintf(stderr, "ERROR: logger: Out of memory\n");
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

static int rotateLogFiles(void)
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
                    fprintf(stderr, "ERROR: logger: Failed to remove file: %s\n", dst);
                }
            }
            if (isFileExists(src)) {
                if (rename(src, dst) != 0) {
                    fprintf(stderr, "ERROR: logger: Failed to rename file: %s -> %s\n", src, dst);
                }
            }
        }
        free(src);
        free(dst);
    }
    s_fl_fp = fopen(s_fl_filename, "a");
    if (s_fl_fp == NULL) {
        fprintf(stderr, "ERROR: logger: Failed to open file: %s\n", s_fl_filename);
        return 0;
    }
    s_fl_currentFileSize = getFileSize(s_fl_filename);
    return 1;
}

static long vflog(enum LogLevel level, FILE* fp, const char* file, int line, const char* func, const char* fmt, va_list arg)
{
    struct timeval tv;
    time_t now;
    char timestr[32];
    const char* levelstr;
    int size;
    long totalsize = 0;

    gettimeofday(&tv, NULL);
    now = tv.tv_sec;
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&now));
    sprintf(timestr, "%s.%06ld", timestr, (long) tv.tv_usec);
    switch (level) {
        case LogLevel_TRACE:
            levelstr = "TRACE";
            break;
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
            levelstr = "ERROR";
            break;
        case LogLevel_FATAL:
            levelstr = "FATAL";
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

void logger_log(enum LogLevel level, const char* file, int line, const char* func, const char* fmt, ...)
{
    va_list arg;

    lock();
    if (!s_initialized) {
        assert(0 && "Not initialized");
        unlock();
        return;
    }
    if (s_logLevel > level) {
        unlock();
        return;
    }
    va_start(arg, fmt);
    if (s_logger == kConsoleLogger) {
        vflog(level, s_cl_stream, file, line, func, fmt, arg);
    } else if (s_logger == kFileLogger) {
        if (rotateLogFiles()) {
            s_fl_currentFileSize += vflog(level, s_fl_fp, file, line, func, fmt, arg);
        }
    } else {
        assert(0 && "Unknown logger");
    }
    va_end(arg);
    unlock();
}
