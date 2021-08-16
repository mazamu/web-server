#include"epoller.h"

Epoller::Epoller(int maxEvent) :_epollerFd(epoll_create(512)),_events(maxEvent){
    assert(_epollerFd >= 0 && _events.size() > 0);
}
Epoller::~Epoller() { close(_epollerFd); }

bool Epoller::addFd(int fd,uint32_t events) {
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(_epollerFd,EPOLL_CTL_ADD,fd,&ev);
}

bool Epoller::modFd(int fd,uint32_t events) {
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(_epollerFd,EPOLL_CTL_MOD,fd,&ev);
}

bool Epoller::delFd(int fd) {
    if(fd < 0) return false;
    epoll_event ev = {0};
    return 0 == epoll_ctl(_epollerFd,EPOLL_CTL_DEL,fd,&ev);
}

int Epoller::wait(int timeoutMs) {
    return epoll_wait(_epollerFd,&_events[0],static_cast<int>(_events.size()),timeoutMs);
}

int Epoller::getEventFd(size_t i) const {
    assert(i < _events.size() && i >= 0);
    return _events[i].data.fd;
}

uint32_t Epoller::getEvents(size_t i) const {
    assert(i < _events.size() && i >= 0);
    return _events[i].events;
}