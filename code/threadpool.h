#ifndef THREADPOOL_H
#define THREADPOOL_H

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
    std::function<void(std::shared_ptr<void>)> process;     // 实际传入的是Server::do_request;
    std::shared_ptr<void> arg;   // 实际应该是HttpData对象
};


class ThreadPool{
public:
    ThreadPool(int threads,int max_queue_size);
    ~ThreadPool();
    bool append(std::function<void(std::shared_ptr<void>)> fun,std::shared_ptr<void> arg);
    void shutdown(bool graceful);
private:
    void run();
    static void* work(void *args);
private:
    //线程同步互斥，
    std::mutex _mtx;
    std::condition_variable _cond;

    int thread_size;
    int max_queue_size;
    int _started;
    int _shutdown;
    std::vector<pthread_t>_threads;
    std::list<ThreadTask>request_queue;
};

#endif