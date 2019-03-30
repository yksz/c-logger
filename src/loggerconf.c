#include "loggerconf.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"

enum {
    /* Logger type */
    kConsoleLogger = 1 << 0,
    kFileLogger = 1 << 1,

    kMaxFileNameLen = 256,
    kMaxLineLen = 512,
};

/* Console logger */
static struct {
    FILE* output;
} s_clog;

/* File logger */
static struct {
    char filename[kMaxFileNameLen];
    long maxFileSize;
    unsigned char maxBackupFiles;
} s_flog;

static int s_logger;

static void reset(void);
static void removeComments(char* s);
static void trim(char* s);
static void parseLine(char* line);
static int hasFlag(int flags, int flag);

int logger_configure(const char* filename)
{
    FILE* fp;
    char line[kMaxLineLen];

    if (filename == NULL) {
        assert(0 && "filename must not be NULL");
        return 0;
    }

    reset();
    if ((fp = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "ERROR: loggerconf: Failed to open file: `%s`\n", filename);
        return 0;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        removeComments(line);
        trim(line);
        if (line[0] == '\0') {
            continue;
        }
        parseLine(line);
    }
    fclose(fp);

    if (hasFlag(s_logger, kConsoleLogger)) {
        if (!logger_initConsoleLogger(s_clog.output)) {
            return 0;
        }
    }
    if (hasFlag(s_logger, kFileLogger)) {
        if (!logger_initFileLogger(s_flog.filename, s_flog.maxFileSize, s_flog.maxBackupFiles)) {
            return 0;
        }
    }
    if (s_logger == 0) {
        return 0;
    }
    return 1;
}

static void reset(void)
{
    s_logger = 0;
    memset(&s_clog, 0, sizeof(s_clog));
    memset(&s_flog, 0, sizeof(s_flog));
}

static void removeComments(char* s)
{
    int i;

    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] == '#') {
            s[i] = '\0';
        }
    }
}

static void trim(char* s)
{
    size_t len;
    int i;

    if (s == NULL) {
        return;
    }
    len = strlen(s);
    if (len == 0) {
        return;
    }
    /* trim right */
    for (i = len - 1; i >= 0 && isspace(s[i]); i--) {}
    s[i + 1] = '\0';
    /* trim left */
    for (i = 0; s[i] != '\0' && isspace(s[i]); i++) {}
    memmove(s, &s[i], len - i);
    s[len - i] = '\0';
}

static LogLevel parseLevel(const char* s);

static void parseLine(char* line)
{
    char *key, *val;
    int nfiles;

    key = strtok(line, "=");
    val = strtok(NULL, "=");

    if (strcmp(key, "level") == 0) {
        logger_setLevel(parseLevel(val));
    } else if (strcmp(key, "autoFlush") == 0) {
        logger_autoFlush(atol(val));
    } else if (strcmp(key, "logger") == 0) {
        if (strcmp(val, "console") == 0) {
            s_logger |= kConsoleLogger;
        } else if (strcmp(val, "file") == 0) {
            s_logger |= kFileLogger;
        } else {
            fprintf(stderr, "ERROR: loggerconf: Invalid logger: `%s`\n", val);
            s_logger = 0;
        }
    } else if (strcmp(key, "logger.console.output") == 0) {
        if (strcmp(val, "stdout") == 0) {
            s_clog.output = stdout;
        } else if (strcmp(val, "stderr") == 0) {
            s_clog.output = stderr;
        } else {
            fprintf(stderr, "ERROR: loggerconf: Invalid logger.console.output: `%s`\n", val);
            s_clog.output = NULL;
        }
    } else if (strcmp(key, "logger.file.filename") == 0) {
        strncpy(s_flog.filename, val, sizeof(s_flog.filename));
    } else if (strcmp(key, "logger.file.maxFileSize") == 0) {
        s_flog.maxFileSize = atol(val);
    } else if (strcmp(key, "logger.file.maxBackupFiles") == 0) {
        nfiles = atoi(val);
        if (nfiles < 0) {
            fprintf(stderr, "ERROR: loggerconf: Invalid logger.file.maxBackupFiles: `%s`\n", val);
            nfiles = 0;
        }
        s_flog.maxBackupFiles = nfiles;
    }
}

static LogLevel parseLevel(const char* s)
{
    if (strcmp(s, "TRACE") == 0) {
        return LogLevel_TRACE;
    } else if (strcmp(s, "DEBUG") == 0) {
        return LogLevel_DEBUG;
    } else if (strcmp(s, "INFO") == 0) {
        return LogLevel_INFO;
    } else if (strcmp(s, "WARN") == 0) {
        return LogLevel_WARN;
    } else if (strcmp(s, "ERROR") == 0) {
        return LogLevel_ERROR;
    } else if (strcmp(s, "FATAL") == 0) {
        return LogLevel_FATAL;
    } else {
        fprintf(stderr, "ERROR: loggerconf: Invalid level: `%s`\n", s);
        return logger_getLevel();
    }
}

static int hasFlag(int flags, int flag)
{
    return (flags & flag) == flag;
}
