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
 #if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
  #include <sys/syscall.h>
  #include <unistd.h>
 #endif /* defined(__linux__) || (defined(__APPLE__) && defined(__MACH__)) */
#endif /* defined(_WIN32) || defined(_WIN64) */

enum
{
    /* Logger type */
    kConsoleLogger = 1,
    kFileLogger = 2,

    kFlushInterval = 10000, /* 1-999999 usec */
    kMaxFileNameLen = 256,
    kDefaultMaxFileSize = 1048576L, /* 1 MB */
};

/* Console logger */
static struct
{
    FILE* output;
}
s_clog;

/* File logger */
static struct
{
    char filename[kMaxFileNameLen];
    FILE* fp;
    long maxFileSize;
    unsigned char maxBackupFiles;
    long currentFileSize;
}
s_flog;

static int s_logger;
static enum LogLevel s_logLevel = LogLevel_INFO;
static struct timeval s_flushtime;
static int s_initialized = 0; /* false */
#if defined(_WIN32) || defined(_WIN64)
static CRITICAL_SECTION s_mutex;
#else
static pthread_mutex_t s_mutex;
#endif /* defined(_WIN32) || defined(_WIN64) */

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

static long getCurrentThreadID()
{
#if defined(_WIN32) || defined(_WIN64)
    return GetCurrentThreadId();
#elif __linux__
    return syscall(SYS_gettid);
#elif defined(__APPLE__) && defined(__MACH__)
    return syscall(SYS_thread_selfid);
#else
    return (long) pthread_self();
#endif /* defined(_WIN32) || defined(_WIN64) */
}

int logger_initConsoleLogger(FILE* output)
{
    output = (output != NULL) ? output : stdout;
    if (output != stdout && output != stderr) {
        assert(0 && "output must be stdout or stderr");
        return 0;
    }

    s_clog.output = output;
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

    if (s_flog.fp != NULL) { /* reinit */
        fclose(s_flog.fp);
    }
    s_flog.fp = fopen(filename, "a");
    if (s_flog.fp == NULL) {
        fprintf(stderr, "ERROR: logger: Failed to open file: %s\n", filename);
        return 0;
    }
    s_flog.currentFileSize = getFileSize(filename);
    strncpy(s_flog.filename, filename, kMaxFileNameLen - 1);
    s_flog.maxFileSize = (maxFileSize > 0) ? maxFileSize : kDefaultMaxFileSize;
    s_flog.maxBackupFiles = maxBackupFiles;
    s_logger = kFileLogger;
    init();
    return 1;
}

void logger_setLevel(enum LogLevel level)
{
    s_logLevel = level;
}

enum LogLevel logger_getLevel(void)
{
    return s_logLevel;
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
        sprintf(backupname, "%.255s", basename);
    } else {
        sprintf(backupname, "%.255s.%d", basename, index);
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

    if (s_flog.currentFileSize < s_flog.maxFileSize) {
        return s_flog.fp != NULL;
    }
    fclose(s_flog.fp);
    for (i = s_flog.maxBackupFiles; i > 0; i--) {
        src = getBackupFileName(s_flog.filename, i - 1);
        dst = getBackupFileName(s_flog.filename, i);
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
    s_flog.fp = fopen(s_flog.filename, "a");
    if (s_flog.fp == NULL) {
        fprintf(stderr, "ERROR: logger: Failed to open file: %s\n", s_flog.filename);
        return 0;
    }
    s_flog.currentFileSize = getFileSize(s_flog.filename);
    return 1;
}

static long vflog(enum LogLevel level, FILE* fp, const char* file, int line, const char* fmt, va_list arg)
{
    struct timeval now;
    char levelc;
    char timestr[32];
    int size;
    long totalsize = 0;

    switch (level) {
        case LogLevel_TRACE:
            levelc = 'T';
            break;
        case LogLevel_DEBUG:
            levelc = 'D';
            break;
        case LogLevel_INFO:
            levelc = 'I';
            break;
        case LogLevel_WARN:
            levelc = 'W';
            break;
        case LogLevel_ERROR:
            levelc = 'E';
            break;
        case LogLevel_FATAL:
            levelc = 'F';
            break;
        default:
            assert(0 && "Unknown LogLevel");
            return 0;
    }
    gettimeofday(&now, NULL);
    strftime(timestr, sizeof(timestr), "%y-%m-%d %H:%M:%S", localtime(&(now.tv_sec)));
    sprintf(timestr, "%s.%06ld", timestr, (long) now.tv_usec);
    if ((size = fprintf(fp, "%c %s %ld %s:%d: ", levelc, timestr, getCurrentThreadID(), file, line)) > 0) {
        totalsize += size;
    }
    if ((size = vfprintf(fp, fmt, arg)) > 0) {
        totalsize += size;
    }
    if ((size = fprintf(fp, "\n")) > 0) {
        totalsize += size;
    }
    if (now.tv_sec - s_flushtime.tv_sec >= 1 || now.tv_usec - s_flushtime.tv_usec > kFlushInterval) {
        fflush(fp);
        s_flushtime.tv_sec = now.tv_sec;
        s_flushtime.tv_usec = now.tv_usec;
    }
    return totalsize;
}

void logger_log(enum LogLevel level, const char* file, int line, const char* fmt, ...)
{
    va_list arg;

    lock();
    if (!s_initialized) {
        assert(0 && "Not initialized");
        goto cleanup;
    }
    if (s_logLevel > level) {
        goto cleanup;
    }
    va_start(arg, fmt);
    switch (s_logger) {
        case kConsoleLogger:
            vflog(level, s_clog.output, file, line, fmt, arg);
            break;
        case kFileLogger:
            if (rotateLogFiles()) {
                s_flog.currentFileSize += vflog(level, s_flog.fp, file, line, fmt, arg);
            }
            break;
        default:
            assert(0 && "Unknown logger");
    }
    va_end(arg);
cleanup:
    unlock();
}
