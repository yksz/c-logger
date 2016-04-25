#ifndef NANOUNIT_H
#define NANOUNIT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <string.h>

static char message[1024];
static int tests_runs = 0;
static int failures = 0;

#define nu_fail() do { \
    sprintf(message, "%s:%d: fail", __FILE__, __LINE__); \
    return 1; \
} while (0)

#define nu_assert(condition) do { \
    if (!condition) { \
        nu_fail(); \
    } \
} while (0)

#define nu_assert_eq_int(expected, actual) do { \
    if (expected != actual) { \
        sprintf(message, "%s:%d: expected:'%d', actual:'%d'", __FILE__, __LINE__, expected, actual); \
        return 1; \
    } \
} while (0)

#define nu_assert_eq_float(expected, actual, delta) do { \
    if (!(expected - delta <= actual && actual <= expected + delta)) { \
        sprintf(message, "%s:%d: expected:'%f', actual:'%f'", __FILE__, __LINE__, expected, actual); \
        return 1; \
    } \
} while (0)

#define nu_assert_eq_ptr(expected, actual) do { \
    if (expected != actual) { \
        sprintf(message, "%s:%d: expected:'%p', actual:'%p'", __FILE__, __LINE__, expected, actual); \
        return 1; \
    } \
} while (0)

#define nu_assert_eq_str(expected, actual) do { \
    if (strcmp(expected, actual) != 0) { \
        sprintf(message, "%s:%d: expected:'%s', actual:'%s'", __FILE__, __LINE__, expected, actual); \
        return 1; \
    } \
} while (0)

#define nu_run_test(test) do { \
    int result = test(); \
    tests_runs++; \
    failures += result; \
    printf("%s ... %s\n", #test, result == 0 ? "passed" : "FAILED"); \
    if (result != 0) { \
        printf("    %s\n", message); \
    } \
} while (0)

#define nu_report() do { \
    printf("\n### %s, %d tests, %d failures\n\n", __FILE__, tests_runs, failures); \
    return failures != 0; \
} while (0)

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* NANOUNIT_H */
