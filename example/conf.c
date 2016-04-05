#include "logger.h"
#include "loggerconf.h"

int main(void) {
    logger_configure("./logger.conf");
    LOG_TRACE("trace");
    LOG_DEBUG("degug");
    LOG_INFO("info");
    LOG_WARN("warn");
    LOG_ERROR("error");
    LOG_FATAL("fatal");
    return 0;
}
