#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <iostream>
#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <thread>
#include"epoller.h"
//#include"threadpool.h"
class WebServer{
public:
    WebServer(int port = 10000);
    ~WebServer();
    void start();
    void initSocket();
    void handleConnection();
    void handleEvent(void* arg);
    void send_data(FILE *fp,char *ct,char *file_name);
    char *content_type(char *file);
    void send_error(FILE *fp);
private:
    Epoller*_epoller;
    //ThreadPool*_threadpool;//线程池负责各个连接的工作
    //std::unordered_map<int,HTTPconnection>_users;//socket对HTTP连接的映射
    int _listenFd;//本身即是主线程，负责监听是否有连接
    int _port;

};

#endif