#include "logger.h"
#include <stdio.h>
#include "nanounit.h"

static int test_trace(void)
{
    /* when: */
    logger_setLevel(LogLevel_TRACE);

    /* then: */
    nu_assert_eq_int(LogLevel_TRACE, logger_getLevel());
    nu_assert(logger_isEnabled(LogLevel_TRACE));
    nu_assert(logger_isEnabled(LogLevel_DEBUG));
    nu_assert(logger_isEnabled(LogLevel_INFO));
    nu_assert(logger_isEnabled(LogLevel_WARN));
    nu_assert(logger_isEnabled(LogLevel_ERROR));
    nu_assert(logger_isEnabled(LogLevel_FATAL));
    return 0;
}

static int test_debug(void)
{
    /* when: */
    logger_setLevel(LogLevel_DEBUG);

    /* then: */
    nu_assert_eq_int(LogLevel_DEBUG, logger_getLevel());
    nu_assert(!logger_isEnabled(LogLevel_TRACE));
    nu_assert(logger_isEnabled(LogLevel_DEBUG));
    nu_assert(logger_isEnabled(LogLevel_INFO));
    nu_assert(logger_isEnabled(LogLevel_WARN));
    nu_assert(logger_isEnabled(LogLevel_ERROR));
    nu_assert(logger_isEnabled(LogLevel_FATAL));
    return 0;
}

static int test_info(void)
{
    /* when: */
    logger_setLevel(LogLevel_INFO);

    /* then: */
    nu_assert_eq_int(LogLevel_INFO, logger_getLevel());
    nu_assert(!logger_isEnabled(LogLevel_TRACE));
    nu_assert(!logger_isEnabled(LogLevel_DEBUG));
    nu_assert(logger_isEnabled(LogLevel_INFO));
    nu_assert(logger_isEnabled(LogLevel_WARN));
    nu_assert(logger_isEnabled(LogLevel_ERROR));
    nu_assert(logger_isEnabled(LogLevel_FATAL));
    return 0;
}

static int test_warn(void)
{
    /* when: */
    logger_setLevel(LogLevel_WARN);

    /* then: */
    nu_assert_eq_int(LogLevel_WARN, logger_getLevel());
    nu_assert(!logger_isEnabled(LogLevel_TRACE));
    nu_assert(!logger_isEnabled(LogLevel_DEBUG));
    nu_assert(!logger_isEnabled(LogLevel_INFO));
    nu_assert(logger_isEnabled(LogLevel_WARN));
    nu_assert(logger_isEnabled(LogLevel_ERROR));
    nu_assert(logger_isEnabled(LogLevel_FATAL));
    return 0;
}

static int test_error(void)
{
    /* when: */
    logger_setLevel(LogLevel_ERROR);

    /* then: */
    nu_assert_eq_int(LogLevel_ERROR, logger_getLevel());
    nu_assert(!logger_isEnabled(LogLevel_TRACE));
    nu_assert(!logger_isEnabled(LogLevel_DEBUG));
    nu_assert(!logger_isEnabled(LogLevel_INFO));
    nu_assert(!logger_isEnabled(LogLevel_WARN));
    nu_assert(logger_isEnabled(LogLevel_ERROR));
    nu_assert(logger_isEnabled(LogLevel_FATAL));
    return 0;
}

static int test_fatal(void)
{
    /* when: */
    logger_setLevel(LogLevel_FATAL);

    /* then: */
    nu_assert_eq_int(LogLevel_FATAL, logger_getLevel());
    nu_assert(!logger_isEnabled(LogLevel_TRACE));
    nu_assert(!logger_isEnabled(LogLevel_DEBUG));
    nu_assert(!logger_isEnabled(LogLevel_INFO));
    nu_assert(!logger_isEnabled(LogLevel_WARN));
    nu_assert(!logger_isEnabled(LogLevel_ERROR));
    nu_assert(logger_isEnabled(LogLevel_FATAL));
    return 0;
}

int main(int argc, char* argv[])
{
    nu_run_test(test_trace);
    nu_run_test(test_debug);
    nu_run_test(test_info);
    nu_run_test(test_warn);
    nu_run_test(test_error);
    nu_run_test(test_fatal);
    nu_report();
}
