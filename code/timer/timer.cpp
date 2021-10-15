#include"timer.h"

// public:
//     Timer_List() { sorted_list.clear(); }
//     ~Timer_List() { sorted_list.clear(); }
//     void add_timer(Timer* t);//将目标定时器添加到链表
//     void adjust_timer(Timer* t);//调整位置，一般来说定时器是延长时间，所以是向后移动
//     void del_timer(Timer* t);//删除定时器
//     void tick();//信号SIGALRM里调用，用来处理到期任务
// private:
//     std::list<Timer>sorted_list;

void Timer_List::add_timer(Timer* t) {
    if(!t) return;

    if(sorted_list.empty()) {
        sorted_list.emplace_back(*t);
        return;
    }

    //如果绝对超时时间最小，则插入头部
    if(*t < *sorted_list.begin()) {
        sorted_list.emplace_front(*t);
        return;
    }
    
    //否则找到 *t < *it，插入*it的前面
    insert_timer(t);
}

void Timer_List::adjust_timer(Timer* t) {
    if(!t) return;
    del_timer(t);
    insert_timer(t);

}

void Timer_List::del_timer(Timer* t) {
    if(!t) return;
    for(auto it = sorted_list.begin(); it != sorted_list.end(); ++it) {
        if(&(*it) == t) {
            sorted_list.erase(it);
            break;
        }
    }
}

void Timer_List::tick() {
    if(sorted_list.empty()) return;
    time_t cur = time(NULL);
    while(!sorted_list.empty()) {
        auto it = sorted_list.begin();
        if(cur < it->expire) break;

        //回调函数
        it->cb_func();
        sorted_list.erase(it);
    }
}