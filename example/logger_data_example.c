#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32) || defined(_WIN64)
 #include <windows.h>
 #define sleep(n) Sleep((n) * 1000)
#else
 #include <unistd.h>
#endif /* defined(_WIN32) || defined(_WIN64) */
#include "logger.h"

int main(void) {
    logger_initDataLogger("127.0.0.1", 9999);
    logger_setLevel(LogLevel_DEBUG);
    for (;;) {
        logger_logData("foo", "Hz", rand() % 60);
        sleep(1);
    }
    return 0;
}
