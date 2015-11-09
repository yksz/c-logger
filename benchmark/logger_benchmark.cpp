#include "logger.h"

int main(void) {
    int count = 1000000;

    logger_useFileLogger("logs/log.txt", 1024 * 1024 * 20, 3);
    for (int i = 0; i < count; i++) {
        LOG_INFO("%d", i);
    }
    return 0;
}
