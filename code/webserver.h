#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include"epoller.h"
#include"threadpool.h"
#include"HTTPconnection.h"
class WebServer{
public:
    WebServer();
    ~WebServer();
    void start();

private:
    std::unique_ptr<Epoller>_epoller;
    std::unique_ptr<ThreadPool>_threadpool;
    int _listenFd;//本身即是主线程，负责监听是否有连接
    

};

#endif