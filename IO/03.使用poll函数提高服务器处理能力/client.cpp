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
#define OPEN_MAX 1000
#define INFTIM -1
#define max(a,b) ((a)>(b))?(a):(b)

static void handle_connection(int sockfd);

int main(int argc,char** argv){
    int connfd = 0;
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    client.sin_addr.s_addr = inet_addr(IPADDRESS);
    //client.sin_addr.s_addr = inet_addr(argv[1]);
    connfd = socket(AF_INET,SOCK_STREAM,0);
    if(connfd < 0) {
        perror("socket \n");
        return -1;
    }
    if(connect(connfd,(struct sockaddr*)&client,sizeof(client))< 0){
        perror("connect \n");
        return -2;
    }
    //处理连接描述符
    handle_connection(connfd);
    return 0;
}

static void handle_connection(int sockfd){
    char sendline[MAXLINE],recvline[MAXLINE];
    //int maxfdp,stdineof;
    struct pollfd pfds[2];
    //添加连接描述符
    pfds[0].fd= sockfd;
    pfds[0].events = POLLIN;
    //添加标准输入描述符
    pfds[1].fd = STDIN_FILENO;
    pfds[1].events = POLLIN;
    int n;
    while(1){
        poll(pfds,2,-1);
        if(pfds[0].revents & POLLIN){
            n =read(sockfd,recvline,MAXLINE);
            if(n==0){
                fprintf(stderr,"client:server is closed.\n");
                close(sockfd);
            }
            write(STDOUT_FILENO,recvline,n);
        }
        //测试标准输入是否准备好
        if(pfds[1].revents & POLLIN){
            n = read(STDIN_FILENO, sendline,MAXLINE);
            if(n==0){
                shutdown(sockfd,SHUT_WR);
                continue;
            }
            write(sockfd,sendline,n);
        }
    }
}
