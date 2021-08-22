#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <iostream>
#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <dirent.h>
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
    
    //开始
    void start();

    //socket相关
    void initSocket();
    void handleConnection();
    void handleEvent(void* arg);
    std::thread wrap_events(void* arg);
    void disconnect(int cliSock);

    //HTTP相关
    void http_request(const char* request,int cliSock);
    void send_respond_head(int cliSock, int no, const char* desp, const char* type, long len);
    void send_file(int cliSock, const char* fileName);
    void send_error(int cliSock, int status, char *title, char *text);
    //译码
    int hexit(char c);
    void encode_str(char* to, int tosize, const char* from);
    void decode_str(char *to, char *from);

    //文件相关
    int get_line(int cliSock,char* line,int size);
    const char *get_file_type(const char *name);

    
private:
    Epoller*_epoller;
    //ThreadPool*_threadpool;//线程池负责各个连接的工作
    //std::unordered_map<int,HTTPconnection>_users;//socket对HTTP连接的映射
    int _listenFd;//本身即是主线程，负责监听是否有连接
    int _port;

};

#endif