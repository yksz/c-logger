#include "logger.h"
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
 #include <io.h>
#else
 #include <fcntl.h>
 #include <unistd.h>
#endif /* defined(_WIN32) || defined(_WIN64) */
#include "nanounit.h"

#if defined(_WIN32) || defined(_WIN64)
#define dup _dup
#define dup2 _dup2
#define fileno _fileno
#endif /* defined(_WIN32) || defined(_WIN64) */

static const char kOutputFilename[] = "console.log";

static void setup(void)
{
    remove(kOutputFilename);
}

static void cleanup(void)
{
    remove(kOutputFilename);
}

static int test_consoleLogger(void)
{
    const char message[] = "message";
    int stdoutfd;
    FILE* redirect;
    int result;
    FILE* fp;
    char line[256];
    int count = 0;

    /* setup: redirect stdout to a file */
    stdoutfd = dup(1);
    if ((redirect = fopen(kOutputFilename, "w")) == NULL) {
        nu_fail();
    }
    dup2(fileno(redirect), 1);

    /* when: */
    result = logger_initConsoleLogger(stdout);

    /* then: */
    nu_assert_eq_int(1, result);

    /* when: output to stdout */
    LOG_FATAL(message);

    /* then: */
    if ((fp = fopen(kOutputFilename, "r")) == NULL) {
        nu_fail();
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strlen(line) - 1] = '\0'; /* remove LF */
        nu_assert_eq_int('F', line[0]);
        nu_assert_eq_str(message, &line[strlen(line) - sizeof(message) + 1]);
        count++;
    }
    nu_assert_eq_int(1, count);

    /* cleanup: restore original stdout */
    dup2(stdoutfd, 1);

    /* and: close resources */
    fclose(fp);
    fclose(redirect);
    close(stdoutfd);
    return 0;
}

int main(int argc, char* argv[])
{
    setup();
    nu_run_test(test_consoleLogger);
    cleanup();
    nu_report();
}
