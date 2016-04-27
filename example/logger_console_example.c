#include "logger.h"

int main(void) {
    logger_initConsoleLogger(stderr);
    logger_setLevel(LogLevel_DEBUG);
    LOG_INFO("console logging");
    return 0;
}
