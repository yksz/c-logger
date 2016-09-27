#include "logger.h"
#if defined(_WIN32) || defined(_WIN64)
 #include <windows.h>
#else
 #include <unistd.h>
 #define Sleep(x) usleep((x) * 1000)
#endif /* defined(_WIN32) || defined(_WIN64) */

static const int kLoggingCount = 10;

int main(void) {
    int i = 0;

    logger_initConsoleLogger(NULL);
    logger_initFileLogger("log.txt", 0, 0);
    LOG_INFO("multi logging");
    logger_setLevel(LogLevel_DEBUG);
    if (logger_isEnabled(LogLevel_DEBUG)) {
        for (i = 0; i < kLoggingCount; i++) {
            LOG_DEBUG("%d", i);
            Sleep(1000);
        }
    }
    return 0;
}
