//
// Created by  huiyuanai on 25-9-20.
//

#include "thread_pool.h"

thread_pool::thread_pool(int num_threads)
    : running(true) {
    threads.reserve(num_threads);

    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread([&]() {
            worker();
        }));
    }
}

thread_pool::~thread_pool() {
    running = false;
    cv.notify_all();
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void thread_pool::add_task(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push_back(std::move(task));
    }
    cv.notify_one(); //通知一个工作线程
}

void thread_pool::worker() {
    while (true) {
        std::function<void()> task; {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return !running || !tasks.empty(); });
            if (!running && tasks.empty()) {
                return; //线程池被销毁且任务队列为空，退出线程
            }
            task = std::move(tasks.front());
            tasks.pop_front();
        }
        task(); //执行任务
    }
}
