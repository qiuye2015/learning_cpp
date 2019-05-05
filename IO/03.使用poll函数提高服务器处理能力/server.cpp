#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<sys/wait.h>
#include<string.h>
#include<errno.h>
#include<poll.h>

#define IPADDRESS "127.0.0.1"
#define PORT 6666
#define MAXLINE 1024
#define LISTENQ 5
//#define OPEN_MAX 1000
#define OPEN_MAX 3
#define INFTIM -1

int bind_and_listen(){
    int serverfd;
    struct sockaddr_in my_addr;
    unsigned int sin_size;
    if((serverfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket \n");
        return -1;
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero),0);

    if(bind(serverfd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr))==-1){
        perror("bind\n");
        return -2;
    }
    if(listen(serverfd,LISTENQ)==-1){
        perror("listen\n");
        return -3;
    }
    return serverfd;
}
//IO多路复用poll
void do_poll(int listenfd){
    //添加监听描述符
    struct pollfd clientfds[OPEN_MAX];
    clientfds[0].fd = listenfd;
    clientfds[0].events = POLLIN;
    int i=0;
    //初始化客户连接描述符
    for(i = 1;i<OPEN_MAX;i++){
        clientfds[i].fd = -1;
    }
    int maxi = 0;
    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen;
    int connfd;
    int nready;
    while(1){
        //获取可用描述符的个数
        nready = poll(clientfds,maxi+1,INFTIM);
        if(nready==-1){
            perror("poll error\n");
            exit(1);
        }
        //测试监听描述符是否准备好
        if(clientfds[0].revents & POLLIN){
            clientaddrlen = sizeof(clientaddr);
            //接收新的连接
            if(connfd =  accept(listenfd,(struct sockaddr*)&clientaddr,&clientaddrlen)==-1){
                if(errno==EINTR) continue;
                else{
                    perror("accept\n");
                    exit(1);
                }
            } 
            fprintf(stdout,"accept a new client :%s:%d\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
            //将新的连接符添加到数组中
            for(i=1;i<OPEN_MAX;++i){
                if(clientfds[i].fd < 0){
                    clientfds[i].fd = connfd;
                    //将新的描述符添加到读描述符集合中
                    clientfds[i].events = POLLIN;
                    break;
                }
            }
            if(i==OPEN_MAX){
                fprintf(stderr,"too many clients.\n");
                exit(1);
            }
            //记录客户连接套接字的个数
            maxi = (i>maxi?i:maxi);
            if(--nready<=0) continue;
        }
        //处理多个连接上客户端发来的包
        char buf[MAXLINE];
        memset(buf,0,MAXLINE);
        int readlen = 0;
        for(i=1;i<=maxi;i++){
            if(clientfds[i].fd < 0) continue;
            //测试客户描述符是否准备好
            if(clientfds[i].revents & POLLIN){
                //接收客户端发送的信息
                readlen = read(clientfds[i].fd,buf,MAXLINE);
                if(readlen==0){
                    close(clientfds[i].fd);
                    clientfds[i].fd = -1;
                    continue;
                }
                write(STDOUT_FILENO,buf,readlen);
                //向客户端发送buf
                write(clientfds[i].fd,buf,readlen);
            }
        }
    }
}

int main(){
    int listenfd = bind_and_listen();
    if(listenfd < 0) return 0;
    do_poll(listenfd);
}
