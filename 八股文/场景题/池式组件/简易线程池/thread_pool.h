//
// Created by  huiyuanai on 25-9-20.
//

#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <thread>
#include <vector>
#include <functional>
#include <deque>
#include <condition_variable>
#include <mutex>
#include <atomic>


class thread_pool {
public:
    thread_pool(int num_threads);
    ~thread_pool();
    void add_task(std::function<void()> task);
private:
    void worker();



    std::condition_variable cv; //通知工作线程有任务了
    std::mutex mtx; //保护任务队列的互斥锁
    std::vector<std::thread> threads;
    std::deque<std::function<void()>> tasks;
    std::atomic<bool> running;
};



#endif //THREAD_POOL_H
