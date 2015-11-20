#include "glog/logging.h"

static const int kLoggingCount = 1000000;

int main(void) {
    FLAGS_logtostderr = 0;
    FLAGS_log_dir = "logs";
    google::InitGoogleLogging("glog");
    for (int i = 0; i < kLoggingCount; i++) {
        LOG(INFO) << i;
    }
    return 0;
}
