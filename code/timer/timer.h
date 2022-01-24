#ifndef TIMER_H
#define TIMER_H

#include<list>
#include<queue>
#include<ctime>
#include<functional>
typedef std::function<void()> TimeoutCallBack;
class Timer {
public:
    int fd;
    time_t expire;//任务的超时时间，使用绝对时间
    TimeoutCallBack cb_func;
    Timer(int socket, TimeoutCallBack, time_t time);
    //void (*cb_func)(int sockfd);
    //std::function<void()> TimeoutCallBack;
    bool operator<(const Timer & t) const {
        return this->expire < t.expire;
    } 
};

class Timer_List {
public:
    Timer_List() {  }
    ~Timer_List() { sorted_list.clear(); }
    void add_timer(Timer* t);//将目标定时器添加到链表
    //void add_timer(int fd,const TimeoutCallBack & cb);
    void adjust_timer(Timer* t);//调整位置，一般来说定时器是延长时间，所以是向后移动
    void del_timer(Timer* t);//删除定时器
    void tick();//信号SIGALRM里调用，用来处理到期任务
    
private:

    std::list<Timer>sorted_list;

    void insert_timer(Timer *t) {
    //否则找到 *t < *it，插入*it的前面
        for(auto it = sorted_list.begin(); it != sorted_list.end(); ++it) {
            if(*it < *t) continue;
            else sorted_list.insert(it,*t);
        }
    }
};

#endif
