#include <unistd.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
std::atomic<bool> start{false};
void threadfunc() {
    while (!start) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    while (start) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "thread func，pid:" << getpid() << std::endl;
    }
}
int main() {
    std::thread t1(threadfunc);
    // daemon(0, 1); // 后台执行
    start.store(true);
    t1.join();  // 等待threadfunc运行结束
    return 0;
}
