#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

#include<unistd.h>
#include<netinet/in.h>
#include<sys/epoll.h>
//#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>

#define IPADDRESS "127.0.0.1"
#define PORT 6666
#define MAXSIZE 1024
#define FDSIZE 1024
#define EPOLLEVENTS 20

//函数声明
//创建socket并连接
int socket_and_connect(const char* ip, int port);
//处理连接
void handle_connection(int sockfd);
//事件处理函数
void handle_events(int epollfd, struct epoll_event *events, int num, int sockfd, char *buf);
//读处理
void do_read(int epollfd, int fd, int sockfd,char *buf);
//写处理
void do_write(int epollfd,int fd, int sockfd,char *buf);
//添加事件
void add_event(int epollfd,int fd,int state);
//修改事件
void modify_event(int epollfd,int fd,int state);
//删除事件
void delete_event(int epollfd,int fd,int state);
int count=0;
int main(int argc, char** argv){
    int sockfd = socket_and_connect(IPADDRESS,PORT);
    handle_connection(sockfd);
    close(sockfd);
    return 0;
}

int socket_and_connect(const char* ip, int port){
    int sockfd;
    //1 创建套接字
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1){
        perror("socket error \n");
        exit(1);
    }
    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    //inet_pton(AF_INET,ip, &serveraddr.sin_addr);//将“点分十进制” －> “二进制整数”
    inet_aton(ip, &serveraddr.sin_addr);//将“点分十进制” －> “二进制整数”
    serveraddr.sin_port = htons(port);//将整型变量从主机字节顺序转变成网络字节顺序
    //2 连接socket
    if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1){
        perror("connect error\n");
        exit(1);
    }
    return sockfd;
}

void handle_connection(int sockfd){
    int epollfd;
    //3 创建一个描述符
    epollfd = epoll_create(FDSIZE);

    //4 添加监听事件描述符
    add_event(epollfd,STDIN_FILENO,EPOLLIN);

    struct epoll_event events[EPOLLEVENTS];
    int ret;
    char buf[MAXSIZE];
    memset(buf,0,MAXSIZE);//同bzero
    while(1){
        //5 获取已经准备好的描述符事件
        ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);//返回需要处理事件的数目; events用于在内核得到事件的集合
        //6 事件处理
        handle_events(epollfd,events,ret,sockfd,buf);
    }
    close(epollfd);
}

void handle_events(int epollfd,struct epoll_event *events,int num,int sockfd,char *buf){
    int fd;
    //进行选号遍历
    for(int i=0;i<num;i++){
        fd = events[i].data.fd;
        //根据描述符类型和事件类型进行处理
        if(events[i].events & EPOLLIN) 
            do_read(epollfd,fd,sockfd,buf);
        else if(events[i].events & EPOLLOUT)
            do_write(epollfd,fd,sockfd,buf);
    }
}

void do_read(int epollfd,int fd,int sockfd,char *buf){
    int nread = read(fd,buf,MAXSIZE);
    if(nread==-1){
        perror("read error \n");
        close(fd);
        delete_event(epollfd,fd,EPOLLIN);
    }
    else if(nread ==0){
        fprintf(stderr,"server close.\n");
        close(fd);
        delete_event(epollfd,fd,EPOLLIN);
    }
    else{
        //printf("read message is : %s\n",buf);
        //modify_event(epollfd,fd,EPOLLOUT);
        if(fd==STDIN_FILENO){//标准输入状态，等待客户端输入字符串
            add_event(epollfd,sockfd,EPOLLOUT);
        }else{//服务器来数据
            delete_event(epollfd,sockfd,EPOLLIN);
            add_event(epollfd,STDOUT_FILENO,EPOLLOUT);
        }
    }
}

void do_write(int epollfd,int fd ,int sockfd,char* buf){
    char temp[100];
    buf[strlen(buf)-1]='\0';
    snprintf(temp,sizeof(temp),"%s_%02d\n",buf,count++);
    int nwrite = write(fd,temp,strlen(temp));
    if(nwrite == -1){
        perror("write error\n");
        close(fd);
    }
    else{
        if(fd==STDOUT_FILENO)//向标准输出写
            delete_event(epollfd,fd,EPOLLOUT);
        else//向网络中写
            modify_event(epollfd,fd,EPOLLIN);
    }
    memset(buf,0,MAXSIZE);
}

void add_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}

void delete_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}

void modify_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}
