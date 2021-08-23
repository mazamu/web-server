# WebServer

### 介绍

该项目是基于C++ 实现的HTTP服务器，也就是那种烂大街的WebServe服务器，虽然github上有很多代码可以参考，但是看别人的代码与自己参考着动手去写的感觉完全不一样，很多地方的一些细节只有自己写过，debug过，才印象深刻。

### 功能

· 利用IO复用技术Epoll与多线程实现Reactor高并发模型。

· 目前支持"GET"方法。

### 使用教程

在目录code下，

编译代码：`make`，编译完成后`./server启动`，注意修改端口号（我的默认是10000）。

删除代码：`make clean`。

### 待改进的地方

· 使用线程池减少创建线程时的损耗。

· 加入定时器关闭非活动的连接。

· 加入日志系统。

· 加入数据库部分，实现用户登录、注册。

### 参考资料

《TCP/IP网络编程》、《Linux高性能服务器》

[编程指北的webserver](https://github.com/imarvinle/WebServer)

[markparticle的高性能服务器](https://github.com/markparticle/WebServer)