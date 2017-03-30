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
 #include <sys/syscall.h>
 #include <unistd.h>
#endif /* defined(_WIN32) || defined(_WIN64) */

enum
{
    /* Logger type */
    kConsoleLogger = 1 << 0,
    kFileLogger = 1 << 1,

    kMaxFileNameLen = 256,
    kDefaultMaxFileSize = 1048576L, /* 1 MB */
    kFlushInterval = 10000, /* 1-999999 usec */
};

/* Console logger */
static struct
{
    FILE* output;
    struct timeval flushtime;
}
s_clog;

/* File logger */
static struct
{
    FILE* output;
    char filename[kMaxFileNameLen];
    long maxFileSize;
    unsigned char maxBackupFiles;
    long currentFileSize;
    struct timeval flushtime;
}
s_flog;

static volatile int s_logger;
static volatile enum LogLevel s_logLevel = LogLevel_INFO;
static volatile int s_initialized = 0; /* false */
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
    tv->tv_sec = (long) (t / 1000000);
    tv->tv_usec = t % 1000000;
    return 0;
}

static struct tm* localtime_r(const time_t* timep, struct tm* result)
{
    localtime_s(result, timep);
    return result;
}
#endif /* defined(_WIN32) || defined(_WIN64) */

static long getCurrentThreadID(void)
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

    init();
    lock();
    s_clog.output = output;
    s_logger |= kConsoleLogger;
    unlock();
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
    int ok = 0; /* false */

    if (filename == NULL) {
        assert(0 && "filename must not be NULL");
        return 0;
    }

    init();
    lock();
    if (s_flog.output != NULL) { /* reinit */
        fclose(s_flog.output);
    }
    s_flog.output = fopen(filename, "a");
    if (s_flog.output == NULL) {
        fprintf(stderr, "ERROR: logger: Failed to open file: `%s`\n", filename);
        goto cleanup;
    }
    s_flog.currentFileSize = getFileSize(filename);
    strncpy(s_flog.filename, filename, kMaxFileNameLen - 1);
    s_flog.maxFileSize = (maxFileSize > 0) ? maxFileSize : kDefaultMaxFileSize;
    s_flog.maxBackupFiles = maxBackupFiles;
    s_logger |= kFileLogger;
    ok = 1; /* true */
cleanup:
    unlock();
    return ok;
}

void logger_setLevel(enum LogLevel level)
{
    s_logLevel = level;
}

enum LogLevel logger_getLevel(void)
{
    return s_logLevel;
}

int logger_isEnabled(enum LogLevel level)
{
    return s_logLevel <= level;
}

static char* getBackupFileName(const char* basename, unsigned char index)
{
    int len = strlen(basename) + 4; /* <basename>.255 */
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

static int isFileExist(const char* filename)
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
    int i;
    char *src, *dst;

    if (s_flog.currentFileSize < s_flog.maxFileSize) {
        return s_flog.output != NULL;
    }
    fclose(s_flog.output);
    for (i = (int) s_flog.maxBackupFiles; i > 0; i--) {
        src = getBackupFileName(s_flog.filename, i - 1);
        dst = getBackupFileName(s_flog.filename, i);
        if (src != NULL && dst != NULL) {
            if (isFileExist(dst)) {
                if (remove(dst) != 0) {
                    fprintf(stderr, "ERROR: logger: Failed to remove file: `%s`\n", dst);
                }
            }
            if (isFileExist(src)) {
                if (rename(src, dst) != 0) {
                    fprintf(stderr, "ERROR: logger: Failed to rename file: `%s` -> `%s`\n", src, dst);
                }
            }
        }
        free(src);
        free(dst);
    }
    s_flog.output = fopen(s_flog.filename, "a");
    if (s_flog.output == NULL) {
        fprintf(stderr, "ERROR: logger: Failed to open file: `%s`\n", s_flog.filename);
        return 0;
    }
    s_flog.currentFileSize = getFileSize(s_flog.filename);
    return 1;
}

static long vflog(FILE* fp, char levelc, const char* timestr, long threadID,
        const char* file, int line, const char* fmt, va_list arg,
        struct timeval* timestamp, struct timeval* flushtime)
{
    int size;
    long totalsize = 0;

    if ((size = fprintf(fp, "%c %s %ld %s:%d: ", levelc, timestr, threadID, file, line)) > 0) {
        totalsize += size;
    }
    if ((size = vfprintf(fp, fmt, arg)) > 0) {
        totalsize += size;
    }
    if ((size = fprintf(fp, "\n")) > 0) {
        totalsize += size;
    }
    if (timestamp->tv_sec - flushtime->tv_sec >= 1
            || timestamp->tv_usec - flushtime->tv_usec > kFlushInterval) {
        fflush(fp);
        flushtime->tv_sec = timestamp->tv_sec;
        flushtime->tv_usec = timestamp->tv_usec;
    }
    return totalsize;
}

static char getLevelChar(enum LogLevel level)
{
    switch (level) {
        case LogLevel_TRACE: return 'T';
        case LogLevel_DEBUG: return 'D';
        case LogLevel_INFO:  return 'I';
        case LogLevel_WARN:  return 'W';
        case LogLevel_ERROR: return 'E';
        case LogLevel_FATAL: return 'F';
        default: return ' ';
    }
}

static void getTimeStr(struct timeval* timestamp, char* timestr, size_t len)
{
    struct tm calendar;

    localtime_r(&timestamp->tv_sec, &calendar);
    strftime(timestr, len, "%y-%m-%d %H:%M:%S", &calendar);
    sprintf(&timestr[17], ".%06ld", (long) timestamp->tv_usec);
}

static int hasFlag(int flags, int flag)
{
    return (flags & flag) == flag;
}

void logger_log(enum LogLevel level, const char* file, int line, const char* fmt, ...)
{
    struct timeval timestamp;
    char levelc;
    char timestr[32];
    long threadID;
    va_list carg, farg;

    if (s_logger == 0 || !s_initialized) {
        assert(0 && "logger is not initialized");
        return;
    }

    if (!logger_isEnabled(level)) {
        return;
    }
    gettimeofday(&timestamp, NULL);
    levelc = getLevelChar(level);
    getTimeStr(&timestamp, timestr, sizeof(timestr));
    threadID = getCurrentThreadID();
    lock();
    if (hasFlag(s_logger, kConsoleLogger)) {
        va_start(carg, fmt);
        vflog(s_clog.output, levelc, timestr, threadID,
                file, line, fmt, carg, &timestamp, &s_clog.flushtime);
        va_end(carg);
    }
    if (hasFlag(s_logger, kFileLogger)) {
        if (rotateLogFiles()) {
            va_start(farg, fmt);
            s_flog.currentFileSize += vflog(s_flog.output, levelc, timestr, threadID,
                    file, line, fmt, farg, &timestamp, &s_flog.flushtime);
            va_end(farg);
        }
    }
    unlock();
}
