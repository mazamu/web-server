#include<assert.h>
#include<sys/prctl.h>

#include"threadpool.h"

ThreadPool::ThreadPool(int threads,int max_queue_size):request_queue(max_queue_size),thread_size(threads),_started(0),_shutdown(0) {
    
    if(threads <= 0 || threads > MAX_QUEUE_SIZE) {
        thread_size = 4;
    }

    if(max_queue_size <= 0 || max_queue_size > MAX_QUEUE_SIZE) {
        max_queue_size = MAX_QUEUE_SIZE;
    }
    //分配空间
    threads.resize(thread_size);

    for(int i = 0; i < thread_size; i++) {
        if(pthread_create(&threads[i],nullptr,worker,this) != 0) {
            std::cout<<"ThreadPool init error"<<std::endl;
            throw::std::exception;
        }
        _started++;   
    }
}

ThreadPool::~ThreadPool() {

}

bool::ThreadPool::append(std::function<void(std::shared_ptr<void>)> fun,std::shared_ptr<void> arg) {
    if(_shutdown) {
        std::cout<<"Thread has shutdown"<<std::endl;
        return false;
    }

    std::unique_ptr<std::mutex>lk(_mtx);
    if(request_queue.size() > max_queue_size) {
        std::cout<<max_queue_size;
        std::cout<<"Thread has too many requests"<<std::endl;
        return false;
    }
    ThreadTask threadTask;
    threadTask.process = fun;
    threadTask.arg = arg;
    request_queue.push_back(threadTask);
    _cond.notify_one();
    return true;
}

void ThreadPool::shutdown(bool graceful) {
    {
        std::unique_lock<std::mutex>lk(_mtx);
        if(_shutdown) {
            std::cout<<"has shutdown"<<std::endl;
        }
        _shutdown = graceful ? graceful_mode : immediate_mode;
        _cond.notify_all();
    }

    for(int i = 0; i < thread_size; i++) {
        if(pthread_join(_threads[i],nullptr) != 0) {
            std::cout<<"pthread_join error"<<std::endl;
        }
    }
}

void* ThreadPool::work(void* args) {
    ThreadPool* pool = static_cast<ThreadPool*>(args);
    //退出线程
    if(pool == nullptr) return nullptr;
    prctl(PR_SET_NAME,"EventLoopThread");

    //执行线程主方法
    pool->run();
    return nullptr;
}

void ThreadPool::run() {
    while(true) {
        ThreadTask requestTask;
        {
            std::unique_lock<std::mutex>lk(_mtx);
            //无任务，且未shutdown则条件等待，注意使用while
            while(request_queue.empty() && !_shutdown) {
                _cond.wait(lk);
            }

            if((_shutdown == immediate_mode) || 
            (_shutdown == graceful_mode && 
            request_queue.empty()) {
                break;
            }

            //FIFO
            requestTask = request_queue.front();
            request_queue.pop_front();
        }
        requestTask.process(requestTask.arg);
    }
}