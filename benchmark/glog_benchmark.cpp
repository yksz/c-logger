#include "glog/logging.h"

int main(void) {
    int count = 1000000;

    FLAGS_logtostderr = 0;
    FLAGS_log_dir = "logs";
    google::InitGoogleLogging("log");
    for (int i = 0; i < count; i++) {
        LOG(INFO) << i;
    }
    return 0;
}
