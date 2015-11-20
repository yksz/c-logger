#include <atomic>
#include <thread>
#include <vector>
#include "glog/logging.h"

static const int kLoggingCount = 1000000;

int main(int argc, char** argv) {
    int numThreads = 10;
    if (argc > 1) {
        numThreads = atoi(argv[1]);
    }

    FLAGS_logtostderr = 0;
    FLAGS_log_dir = "logs";
    google::InitGoogleLogging("glog");

    std::atomic<int> count(0);
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++) {
        threads.push_back(std::thread([&]() {
            while (count < kLoggingCount) {
                LOG(INFO) << count++;
            }
        }));
    }
    for (std::thread& th : threads) {
        th.join();
    }
    return 0;
}

