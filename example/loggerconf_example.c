#include <stdio.h>
#include "logger.h"
#include "loggerconf.h"

int main(int argc, char* argv[]) {
    char filename[256];
    if (argc <= 1) {
        printf("usage: %s <conf file>\n", argv[0]);
        return 1;
    }
    strncpy(filename, argv[1], strlen(argv[1]));

    logger_configure(filename);
    LOG_TRACE("trace");
    LOG_DEBUG("degug");
    LOG_INFO("info");
    LOG_WARN("warn");
    LOG_ERROR("error");
    LOG_FATAL("fatal");
    return 0;
}
