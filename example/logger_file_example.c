#include "logger.h"

int main(void) {
    logger_initFileLogger("log.txt", 1024 * 1024, 5);
    logger_setLevel(LogLevel_DEBUG);
    LOG_INFO("file logging");
    LOG_DEBUG("format example: %d%c%s", 1, '2', "3");
    return 0;
}
