#include "thread_pool_test.h"
#include "thread_pool.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

void test_basic_task() {
    thread_pool pool(4);
    std::atomic<int> counter{0};
    for (int i = 0; i < 10; ++i) {
        pool.add_task([&counter]() {
            counter++;
        });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Basic task test, counter: " << counter << std::endl;
}

void test_concurrent_tasks() {
    thread_pool pool(4);
    std::vector<int> results(8, 0);
    for (int i = 0; i < 8; ++i) {
        pool.add_task([i, &results]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            results[i] = i * i;
        });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    std::cout << "Concurrent tasks test: ";
    for (auto v : results) std::cout << v << " ";
    std::cout << std::endl;
}
