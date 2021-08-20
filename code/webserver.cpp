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
    setsockopt(_listenFd,SOL_SOCKET,SO_REUSEADDR,(void*)option,optlen);

    bzero(&_addr,sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bind_num = bind(_listenFd,(struct sockaddr*)&_addr,sizeof(_addr));
    assert(bind_num != -1);
    int listen_num = listen(_listenFd,512);
    assert(listen_num != -1);

    _epoller->addFd(_listenFd,EPOLLIN | EPOLLET);//发生在listenFd上才进行accept
}

void WebServer::start() {
    initSocket();

    int event_num = 0;
    //pthread_t tid;
    while(1) {
        event_num = _epoller->wait();
        for(int i = 0; i < event_num; i++) {
            int fd = _epoller->getEventFd(i);
            if(fd == _listenFd) {//连接事件
                std::cout<<"listenFd event"<<std::endl;
                handleConnection();
            }else {
                int event = _epoller->getEvents(i);
                if(event & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                    //关闭相应连接
                    std::cout<<"event事件错误"<<std::endl;
                    exit(0);
                }else if(event & EPOLLIN) {
                    //读事件
                    std::cout<<"EPOLLIN event"<<std::endl;
                    handleEvent((void*)&fd);
                    //std::thread(&WebServer::handleEvent,this,&fd);
                    //_threadpool->append(std::bind(&WebServer::handleEvent,this,std::placeholders::_1),&fd);
                }else if(event & EPOLLOUT) {
                    std::cout<<"EPOLLOUT event"<<std::endl;
                }else {}
            }
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
    _epoller->addFd(cliSock,EPOLLIN | EPOLLET | EPOLLONESHOT);
}

void WebServer::handleEvent(void* arg) {
    int clnt_sock = static_cast<int>(*(int*)arg);
    char req_line[SMALL_BUF];
    FILE *clnt_read;
    FILE *clnt_write;

    char method[10];
    char ct[15];
    char file_name[30];

    clnt_read = fdopen(clnt_sock, "r");
    clnt_write = fdopen(dup(clnt_sock), "w");
    fgets(req_line, SMALL_BUF, clnt_read);
    if (strstr(req_line, "HTTP/") == NULL)
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return;
    }
    strcpy(method, strtok(req_line, " /"));
    strcpy(file_name, strtok(NULL, " /"));
    strcpy(ct, content_type(file_name));
    if (strcmp(method, "GET") != 0)
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return;
    }
    fclose(clnt_read);
    send_data(clnt_write, ct, file_name);

}

void WebServer::send_data(FILE *fp, char *ct, char *file_name)
{
    char protocol[] = "HTTP/1.1 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[SMALL_BUF];
    char buf[BUF_SIZE];
    FILE *send_file;

    sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
    send_file = fopen(file_name, "r");
    if (send_file == NULL)
    {
        send_error(fp);
        return;
    }

    //传输头信息
    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);

    //传输请求数据
    while (fgets(buf, BUF_SIZE, send_file) != NULL)
    {
        fputs(buf, fp);
        fflush(fp);
    }
    fflush(fp);
    fclose(fp);
}

char* WebServer::content_type(char *file)
{
    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name, file);
    strtok(file_name, ".");
    strcpy(extension, strtok(NULL, "."));

    if (!strcmp(extension, "html") || !strcmp(extension, "htm"))
        return "text/html";
    else
        return "text/plain";
}
void WebServer::send_error(FILE *fp)
{
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[] = "Content-type:text/html\r\n\r\n";
    char content[] = "<html><head><title>NETWORK</title></head>"
                     "<body><font size=+5><br>发生错误！ 查看请求文件名和请求方式!"
                     "</font></body></html>";
    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);
    fflush(fp);
}