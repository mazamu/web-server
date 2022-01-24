#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <iostream>
#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
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
#include <unordered_map>
#include"epoller.h"
#include"logs/log.h"
//#include"timer/timer.h"
//#include"threadpool.h"
#include"ThreadPool.h"
class WebServer{
public:
    WebServer(int port = 10000);
    ~WebServer();
    
    //开始
    void start();

    //socket相关
    void initSocket();//初始化监听socket
    void handleConnection();//处理_listenFd连接事件
    void handleEvent(void* arg);//处理HTTP事件
    std::thread wrap_events(void* arg);//对handleEvent包装，然后可以使用C++11的多线程
    void disconnect(int cliSock);//处理非活动连接

    //HTTP相关
    void http_request(const char* request,int cliSock);
    void send_respond_head(int cliSock, int no, const char* desp, const char* type, long len);//发消息头
    void send_file(int cliSock, const char* fileName);//发内容
    void send_error(int cliSock, int status, char *title, char *text);
    
    // //译码
    // int hexit(char c);
    // void encode_str(char* to, int tosize, const char* from);
    // void decode_str(char *to, char *from);

    //文件相关
    int get_line(int cliSock,char* line,int size);//获取一行内容
    const char *get_file_type(const char *name);//得到参数的类型
    void setnonBlocking(int fd);//设置非阻塞I/O
    
    //信号相关
    // void handleSig(bool &timeout);//处理信号主函数
    // //void sendSig(int sig);//发信号
    // void addSig(int sig);//添加信号
    // void handleTimer();//定时处理任务，内部调用tick()
    
    // static int pipeFd[2];


private:

    int _listenFd;//本身即是主线程，负责监听是否有连接
    int _port;

    Epoller*_epoller;
    ThreadPool *_pool;
    //ThreadPool*_threadpool;//线程池负责各个连接的工作
    //std::unordered_map<int,HTTPconnection>_users;//socket对HTTP连接的映射

    // std::unordered_map<int,Timer*>_timerMap;//cliSock对定时器的映射
    // Timer_List *_timerList;//定时器升序链表
    //bool timeout;//标志是否有超时

};

#endif
