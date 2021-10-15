#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<stdio.h>
#include<vector>
#include<list>
#include<functional>
#include<pthread.h>
#include<memory>
#include<mutex>
#include<condition_variable>
const int MAX_THREAD_SIZE = 1024;
const int MAX_QUEUE_SIZE = 10000;

typedef enum {
    immediate_mode = 1,
    graceful_mode = 2
} ShutdownMode;

struct ThreadTask {
    std::function<void(void*)> process;     // 相应函数;
    void* arg;   // 相应参数
};


class ThreadPool{
public:
    ThreadPool(int threads = 4,int max_queue_size = 1024);
    ~ThreadPool();
    bool append(std::function<void(void*)> fun,void* arg);
    void shutdown(bool graceful);
    //void test(std::shared_ptr<void>);
private:
    void run();
    static void* work(void *args);
private:
    //线程同步互斥，
    std::mutex _mtx;
    std::condition_variable _cond;

    int thread_size;
    int max_queue_size;
    int _shutdown;
    std::vector<pthread_t>_threads;
    std::list<ThreadTask>request_queue;
};

#endif