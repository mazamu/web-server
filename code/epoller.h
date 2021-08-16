#ifndef EPOLLER_H
#define EPOLLER_H

#include<sys/epoll.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<vector>
#include<errno.h>

class Epoller{
public:
    explicit Epoller(int maxEvent = 1024);
    ~Epoller();

    bool addFd(int fd,uint32_t events);//
    bool modFd(int fd,uint32_t events);
    bool delFd(int fd);
    int wait(int timewait = -1);
    int getEventFd(size_t i) const;
    uint32_t getEvents(size_t i) const;

private:
    int _epollerFd;
    std::vector<struct epoll_event>_events;
};

#endif //EPOLLER_H