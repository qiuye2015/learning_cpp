#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAXLINE 4096

int main(int argc, char** argv){
    int sockfd,n;
    char recvline[4096],sendline[4096];
    struct sockaddr_in serveradd;

    if(argc !=2){
        printf("usage: ./clinet <IP>\n");
        return 0;
    }
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) <0){
        printf("create socket error: %s(error %d)\n", strerror(errno),errno);
        return 0;
    }

    memset(&serveradd,0,sizeof(serveradd));
    serveradd.sin_family =AF_INET;
    serveradd.sin_port = htons(6666);
    if(inet_pton(AF_INET,argv[1],&serveradd.sin_addr) <=0){
        printf("inet_pton error: %s(error %d)\n", strerror(errno),errno);
        return 0;
    }
    if(connect(sockfd,(struct sockaddr*)&serveradd,sizeof(serveradd)) <0 ){
        printf("connect error %s (errno:%d)\n",strerror(errno),errno);
        return 0;
    }
    printf("send msg to server: \n");
    fgets(sendline,4096,stdin);
    if(send(sockfd,sendline,strlen(sendline),0) <  0){
        printf("send msg error: %s(error %d)\n", strerror(errno),errno);
        return 0;
    }
    close(sockfd);
    return 0;
}
