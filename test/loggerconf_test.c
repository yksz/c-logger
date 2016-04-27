#include "loggerconf.h"
#include "logger.h"
#include "nanounit.h"

static void cleanup(void)
{
    remove("conf.log");
}

static int test_configure_empty(void)
{
    int result = logger_configure("res/empty.conf");
    nu_assert_eq_int(0, result);
    return 0;
}

static int test_configure_consoleLogger(void)
{
    int result = logger_configure("res/console.conf");
    nu_assert_eq_int(1, result);
    nu_assert_eq_int(LogLevel_TRACE, logger_getLevel());
    return 0;
}

static int test_configure_fileLogger(void)
{
    int result = logger_configure("res/file.conf");
    nu_assert_eq_int(1, result);
    nu_assert_eq_int(LogLevel_DEBUG, logger_getLevel());
    return 0;
}

int main(int argc, char* argv[])
{
    nu_run_test(test_configure_empty);
    nu_run_test(test_configure_consoleLogger);
    nu_run_test(test_configure_fileLogger);
    cleanup();
    nu_report();
}
