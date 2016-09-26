#include "logger.h"

int main(void) {
    logger_initConsoleLogger(NULL);
    logger_initFileLogger("log.txt", 0, 0);
    LOG_INFO("multi logging");
    if (logger_isEnabled(LogLevel_DEBUG)) {
        LOG_DEBUG("debug");
    }
    return 0;
}
