# WebServer

### 介绍

该项目是基于C++ 实现的HTTP服务器，也就是那种烂大街的WebServe服务器，虽然github上有很多代码可以参考，但是看别人的代码与自己参考着动手去写的感觉完全不一样，很多地方的一些细节只有自己写过，debug过，才印象深刻。

### 功能

+ 使用epoll + 非阻塞IO + 边缘触发(ET) 实现高并发处理请求，使用Reactor编程模型

+ epoll使用EPOLLONESHOT保证一个socket连接在任意时刻都只被一个线程处理

+ 添加定时器支持HTTP长连接，定时回调handler处理超时连接

+ 使用C++标准库双向链表list来管理定时器

+ 使用epoll与管道结合管理定时信号

+ 目前支持GET方法

### 目录结构


```
.
├── epoller.cpp
├── epoller.h
├── index.html
├── logs
│   ├── log.cpp
│   └── log.h
├── main.cpp
├── Makefile
├── svtrr
├── threadpool
│   ├── threadpool.cpp
│   └── threadpool.h
├── timer
│   ├── timer.cpp
│   └── timer.h
├── webserver.cpp
└── webserver.h
```


### 使用教程

在目录code下

编译代码：`make`，编译完成后`./svtrr启动`，注意修改端口号（我的默认是10000）

删除代码：`make clean`

### 待改进的地方

+ 支持post方法

+ 使用线程池减少创建线程时的损耗

+ 加入数据库部分，实现用户登录、注册

# 压力测试
- https://github.com/mazamu/web-server/tree/master/pressureTest

### 参考资料

《TCP/IP网络编程》、《Linux高性能服务器》

[编程指北的webserver](https://github.com/imarvinle/WebServer)

[markparticle的高性能服务器](https://github.com/markparticle/WebServer)
