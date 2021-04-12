#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

#include<unistd.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>

#define IPADDRESS "127.0.0.1"
#define PORT 6666
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

//函数声明
//创建套接字并绑定
int bind_and_listen(const char* ip, int port);
//IO 多路复用epoll
void do_epoll(int listenfd);
//事件处理函数
void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char *buf);
//处理接受到的连接
void handle_accept(int epollfd, int listenfd);
//读处理
void do_read(int epollfd, int fd, char *buf);
//写处理
void do_write(int epollfd,int fd, char *buf);
//添加事件
void add_event(int epollfd,int fd,int state);
//修改事件
void modify_event(int epollfd,int fd,int state);
//删除事件
void delete_event(int epollfd,int fd,int state);

int main(int argc, char** argv){
    int listenfd;
    listenfd = bind_and_listen(IPADDRESS,PORT);
    do_epoll(listenfd);

    return 0;
}

int bind_and_listen(const char* ip, int port){
    int listenfd;
    //1 创建套接字
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd==-1){
        perror("socket error \n");
        exit(1);
    }
    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    //inet_pton(AF_INET,ip, &serveraddr.sin_addr);//将“点分十进制” －> “二进制整数”
    inet_aton(ip, &serveraddr.sin_addr);//将“点分十进制” －> “二进制整数”
    serveraddr.sin_port = htons(port);//将整型变量从主机字节顺序转变成网络字节顺序
    //2 绑定套接字
    if(bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1){
        perror("bind error\n");
        exit(1);
    }
    //3 监听套接字
    if(listen(listenfd,LISTENQ) == -1 ){
        perror("listen error\n");
        exit(1);
    }
    return listenfd;
}

void do_epoll(int listenfd){
    int epollfd;
    //4 创建一个描述符
    epollfd = epoll_create(FDSIZE);

    //5 添加监听事件描述符
    add_event(epollfd,listenfd,EPOLLIN);

    struct epoll_event events[EPOLLEVENTS];//100
    int ret;
    char buf[MAXSIZE];
    memset(buf,0,MAXSIZE);//同bzero
    while(1){
        //6 获取已经准备好的描述符事件
        ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);//100 返回需要处理事件的数目; events用于在内核得到事件的集合
        //7 事件处理
        handle_events(epollfd,events,ret,listenfd,buf);
    }
    close(epollfd);
}

void handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char *buf){
    int fd;
    //进行选号遍历
    for(int i=0;i<num;i++){
        fd = events[i].data.fd;
        //根据描述符类型和事件类型进行处理
        if((fd == listenfd) && (events[i].events & EPOLLIN))
            handle_accept(epollfd,fd);
       else if(events[i].events & EPOLLIN) 
           do_read(epollfd,fd,buf);
       else if(events[i].events & EPOLLOUT)
           do_write(epollfd,fd,buf);
    }
}

void handle_accept(int epollfd,int listenfd){
    struct sockaddr_in newaddr;
    socklen_t newaddrlen;
    //8 接收新的连接
    int newfd =accept(listenfd,(struct sockaddr *)&newaddr,&newaddrlen);
    //从socket fd 请求队列抽取第一个连接信息，创立一个与fd同类新的socket返回句柄，否则阻塞
    
    if(newfd ==-1){
        perror("accept error\n");
    }
    else{
        //inet_ntoa 将一个IP转换成一个互联网标准点分格式的字符串 inet_aton反之
        printf("accept a new client :%s:%d\n",inet_ntoa(newaddr.sin_addr),newaddr.sin_port);
        //9 同5 添加一个客户描述符和事件
        add_event(epollfd,newfd,EPOLLIN);
    }
}

void do_read(int epollfd,int fd,char *buf){
    //10 在fd中读数据
    int nread = read(fd,buf,MAXSIZE);
    if(nread==-1){
        perror("read error \n");
        close(fd);
        //11 删除事件
        delete_event(epollfd,fd,EPOLLIN);
    }
    else if(nread ==0){
        fprintf(stderr,"client close.\n");
        close(fd);
        delete_event(epollfd,fd,EPOLLIN);
    }
    else{
        printf("read message is : %s\n",buf);
        //12 修改事件
        modify_event(epollfd,fd,EPOLLOUT);
    }
}

void do_write(int epollfd,int fd ,char* buf){
    //13 往fd中写数据
    int nwrite = write(fd,buf,strlen(buf));
    if(nwrite == -1){
        perror("write error\n");
        close(fd);
        delete_event(epollfd,fd,EPOLLOUT);
    }
    else
        modify_event(epollfd,fd,EPOLLIN);
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
