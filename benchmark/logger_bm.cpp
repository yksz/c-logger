#include "logger.h"

static const int kLoggingCount = 1000000;

int main(void) {
    logger_initFileLogger("logs/logger.txt", 1024 * 1024 * 30, 3);
    for (int i = 0; i < kLoggingCount; i++) {
        LOG_INFO("%d", i);
    }
    return 0;
}
