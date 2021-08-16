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
#include"HTTPconnection.h"
class WebServer{
public:
    WebServer();
    ~WebServer();
    void start();

private:
    std::unique_ptr<Epoller>_epoller;
};

#endif