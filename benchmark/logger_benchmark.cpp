#include "logger.h"

int main(void) {
    int count = 1000000;

    logger_init(LogLevel_INFO, "logs/log.txt");
    for (int i = 0; i < count; i++) {
        LOG_INFO("%d", i);
    }
    return 0;
}
