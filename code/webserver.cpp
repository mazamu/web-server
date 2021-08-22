#include"webserver.h"
const int SMALL_BUF = 100;
const int BUF_SIZE = 1024;
WebServer::WebServer(int port):_listenFd(-1),_port(port),_epoller(new Epoller()) {

}

WebServer::~WebServer() {
    delete _epoller;
    //delete _threadpool;
}

void WebServer::initSocket() {
    _listenFd = socket(PF_INET,SOCK_STREAM,0);
    sockaddr_in _addr;
    
    //去除time_wait，仅供测试用
    int option,optlen;
    option = 1;
    optlen = sizeof(option);
    setsockopt(_listenFd,SOL_SOCKET,SO_REUSEADDR,&option,optlen);

    bzero(&_addr,sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bind_num = bind(_listenFd,(struct sockaddr*)&_addr,sizeof(_addr));
    assert(bind_num != -1);
    int listen_num = listen(_listenFd,512);
    assert(listen_num != -1);

    _epoller->addFd(_listenFd,EPOLLIN);
}

void WebServer::start() {
    initSocket();

    int event_num = 0;
    //pthread_t tid;
    while(1) {
        event_num = _epoller->wait();
        for(int i = 0; i < event_num; i++) {
            int fd = _epoller->getEventFd(i);
            uint32_t event = _epoller->getEvents(i);

            if(fd == _listenFd) {
                //连接事件
                std::cout<<"listenFd event"<<std::endl;
                handleConnection();
            }else {
                if(event & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                    //关闭相应连接
                    std::cout<<"event事件错误"<<std::endl;
                    exit(0);
                }else if(event & EPOLLIN) {
                    //读事件
                    //pthread_t tid;
                    std::cout<<"EPOLLIN event"<<std::endl;
                    //handleEvent((void*)&fd);
                    wrap_events((void*)&fd).detach();
                    //std::thread(&WebServer::handleConnection,(void*)&fd);
                    //pthread_create(&tid,NULL,&(WebServer::handleEvent),(void*)&fd);
                    //std::thread(&WebServer::handleEvent,this,&fd);
                    //_threadpool->append(std::bind(&WebServer::handleEvent,this,std::placeholders::_1),&fd);
                }else if(event & EPOLLOUT) {
                    std::cout<<"EPOLLOUT event"<<std::endl;
                }else {}
            }

            // if(!(pev->events & EPOLLIN)) {
             
            //     // 不是读事件
            //     continue;
            // }
            // if(pev->data.fd == lfd){
             
            //     // 接受连接请求
            //     do_accept(lfd, epfd);
            // } else {
            
            //     // 读数据
            //     printf("======================before do read, ret = %d\n", ret);
            //     do_read(pev->data.fd, epfd);
            //     printf("=========================================after do read\n");
            // }
        }
    }
}

void WebServer::handleConnection() {
    //处理_listenFd上的连接
    int cliSock = -1;
    sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    memset(&cli_addr,0,sizeof(cli_addr));
    cliSock = accept(_listenFd,(struct sockaddr*)&cli_addr, &cli_len);
    assert(cliSock > 0);

    //设置cliSock为非阻塞模式
    int flag = fcntl(cliSock,F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cliSock,F_SETFL,flag);

    //把cliSock添加到epoller上
    _epoller->addFd(cliSock,EPOLLIN | EPOLLET | EPOLLONESHOT);
}

std::thread WebServer::wrap_events(void* arg) {
    return std::thread(&WebServer::handleEvent,this,arg);
}

void WebServer::handleEvent(void* arg) {
    int cliSock = *(int*)arg;
    // 将浏览器发过来的数据, 读到buf中 
    char line[1024] = {0};
    // 读请求行
    int len = get_line(cliSock, line, sizeof(line));
    if(len == 0) {   
        printf("客户端断开了连接...\n");
        // 关闭套接字, cliSock从epoll上del
        disconnect(cliSock);         
    } else { 
    	printf("============= 请求头 ============\n");   
        printf("请求行数据: %s", line);
        // 还有数据没读完,继续读走
		while (1) {
			char buf[1024] = {0};
			len = get_line(cliSock, buf, sizeof(buf));	
			if (buf[0] == '\n') {
				break;	
			} else if (len == -1)
				break;
		}
        printf("============= The End ============\n");
    }
    
    // 判断get请求
    if(strncasecmp("get", line, 3) == 0) { // 请求行: get /hello.c http/1.1   
        // 处理http请求
        http_request(line, cliSock);
        
        // 关闭套接字, cliSock从epoll上del
        disconnect(cliSock);         
    }
}

void WebServer::disconnect(int cliScok) {
    _epoller->delFd(cliScok);
    close(cliScok);
}

// 解析http请求消息的每一行内容
int WebServer::get_line(int sock, char *buf, int size) {
    int i = 0;
    char c = '\0';
    while((i < size - 1) && (c != '\n')) {
        int n = recv(sock,&c,1,0);
        if(n > 0) {
            if(c == '\r') {

                n = recv(sock,&c,1,MSG_PEEK);//查看下一个字符是不是'\n'，但不取出缓冲区
                if((n > 0) && (c == '\n')) recv(sock,&c,1,0);
                else c = '\n';

            }

            buf[i] = c;
            i++;
        }else c = '\n';
    }

    buf[i] = '\0';
    return i;
}

void WebServer::http_request(const char* request,int cliScok) {
    char method[12], path[1024], protocol[12];
    sscanf(request, "%[^ ] %[^ ] %[^ ]", method, path, protocol);
    printf("method = %s, path = %s, protocol = %s\n", method, path, protocol);   

    // 转码 将不能识别的中文乱码 -> 中文
    // 解码 %23 %34 %5f
    decode_str(path, path); 

    char* file = path + 1; // 去掉path中的/ 获取访问文件名
    
    // 如果没有指定访问的资源, 默认显示资源目录中的内容
    if(strcmp(path, "/") == 0) {    
        // file的值, 资源目录的当前位置
        file = "./";
    }

    // 获取文件属性
    struct stat st;
    int ret = stat(file, &st);
    if(ret == -1) { 
        send_error(cliScok, 404, "Not Found", "No such file");     
        return;
    }

    // 判断是目录还是文件
    if(S_ISREG(st.st_mode)) { // 文件        
        // 发送消息报头
        send_respond_head(cliScok, 201, "OK", get_file_type(file), st.st_size);
        // 发送文件内容
        send_file(cliScok, file);
    }else if(S_ISDIR(st.st_mode)) {
        send_error(cliScok, 404, "Not Found", "you enter a dir");   
    } 

}

void WebServer::send_respond_head(int cliSock, int no, const char* desp, const char* type, long len) {
    char buf[1024] = {0};
    // 状态行
    sprintf(buf, "http/1.1 %d %s\r\n", no, desp);
    send(cliSock, buf, strlen(buf), 0);
    // 消息报头
    sprintf(buf, "Content-Type:%s\r\n", type);
    sprintf(buf+strlen(buf), "Content-Length:%ld\r\n", len);
    send(cliSock, buf, strlen(buf), 0);
    // 空行
    send(cliSock, "\r\n", 2, 0);

}

void WebServer::send_file(int cliSock,const char* fileName) {
    
    //打开文件
    int fd = open(fileName,O_RDONLY);
    if(fd == -1) {
        send_error(cliSock,404,"Not Found","No such file");
        exit(1);
    }

    //循环读文件
    char buf[4096] = {0};
    int len = 0,ret = 0;
    while( (len = read(fd,buf,sizeof(buf))) > 0 ) {
        ret = send(cliSock,buf,len,0);
        if(ret == -1) {
            if(errno == EAGAIN) {
                perror("send error:");
                continue;
            }else if(errno == EINTR) {
                perror("send error:");
                continue;
            }else {
                perror("send error:");
                exit(1);
            }
        }
    }

    if(len == -1) {
        perror("read file error");
        exit(1);
    }

    close(fd);
}

const char *WebServer::get_file_type(const char *name){
    const char* dot;

    // 自右向左查找‘.’字符, 如不存在返回NULL
    dot = strrchr(name, '.');   
    if (dot == NULL)
        return "text/plain; charset=utf-8";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp( dot, ".wav" ) == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";

    return "text/plain; charset=utf-8";
}

void WebServer::send_error(int cliSock, int status, char *title, char *text){
	char buf[4096] = {0};

	sprintf(buf, "%s %d %s\r\n", "HTTP/1.1", status, title);
	sprintf(buf+strlen(buf), "Content-Type:%s\r\n", "text/html");
	sprintf(buf+strlen(buf), "Content-Length:%d\r\n", -1);
	sprintf(buf+strlen(buf), "Connection: close\r\n");
	send(cliSock, buf, strlen(buf), 0);
	send(cliSock, "\r\n", 2, 0);

	memset(buf, 0, sizeof(buf));

	sprintf(buf, "<html><head><title>%d %s</title></head>\n", status, title);
	sprintf(buf+strlen(buf), "<body bgcolor=\"#cc99cc\"><h2 align=\"center\">%d %s</h4>\n", status, title);
	sprintf(buf+strlen(buf), "%s\n", text);
	sprintf(buf+strlen(buf), "<hr>\n</body>\n</html>\n");
	send(cliSock, buf, strlen(buf), 0);
	
	return ;
}

/*
 *  这里的内容是处理%20之类的东西！是"解码"过程。
 *  %20 URL编码中的‘ ’(space)
 *  %21 '!' %22 '"' %23 '#' %24 '$'
 *  %25 '%' %26 '&' %27 ''' %28 '('......
 *  相关知识html中的‘ ’(space)是&nbsp
 */
void WebServer::encode_str(char* to, int tosize, const char* from){
    int tolen;

    for (tolen = 0; *from != '\0' && tolen + 4 < tosize; ++from) {    
        if (isalnum(*from) || strchr("/_.-~", *from) != (char*)0) {      
            *to = *from;
            ++to;
            ++tolen;
        } else {
            sprintf(to, "%%%02x", (int) *from & 0xff);
            to += 3;
            tolen += 3;
        }
    }
    *to = '\0';
}

void WebServer::decode_str(char *to, char *from){
    for ( ; *from != '\0'; ++to, ++from  ) {     
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) {       
            *to = hexit(from[1])*16 + hexit(from[2]);
            from += 2;                      
        } else {
            *to = *from;
        }
    }
    *to = '\0';
}

int WebServer::hexit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return 0;
}