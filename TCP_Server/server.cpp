#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#define MAXLINE 4096

int main(int argc, char** argv){
    int listenfd, connfd;
    struct sockaddr_in serveraddr;
    char buff[4096];
    int n;

    if((listenfd=socket(AF_INET, SOCK_STREAM,0)) == -1){
        printf("create socket error: %s(error %d)\n", strerror(errno),errno);
        return 0;
    }
    memset(&serveraddr, 0 ,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(6666);

    if(bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1){
        printf("bind socket error: %s(error %d)\n", strerror(errno),errno);
        return 0;
    }

    if(listen(listenfd,10)==-1){
        printf("listen socket error %s(error %d)\n", strerror(errno),errno);
        return 0;
    }

    printf("===========waiting for client's request==========\n");
    while(1){
        if((connfd= accept(listenfd,(struct sockaddr*)NULL,NULL)) == -1 ){
            printf("accept socket error: %s(error %d)\n", strerror(errno),errno);
            continue;
        }
        n = recv(connfd,buff,MAXLINE,0);
        buff[n]='\0';
        printf("recv msg from clone: %s\n",buff);
        close(connfd);
    }

    close(listenfd);

}
