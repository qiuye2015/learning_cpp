#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define DEFAULT_PORT 6666

int main(int argc,char **argv){
    int conn_fd = 0;
    int cLen = 0;
    struct sockaddr_in client;

    if(argc < 2){
        printf(" Usage: client [IP]\n");
        return -1;
    }

    client.sin_family = AF_INET;
    client.sin_port = htons(DEFAULT_PORT);
    client.sin_addr.s_addr = inet_addr(argv[1]);

    conn_fd = socket(AF_INET,SOCK_STREAM,0);
    if(conn_fd<0){
        perror("socket \n");
    }

    if(connect(conn_fd,(struct sockaddr*) &client,sizeof(client)) <0 ){
        perror("connect \n");
        return -2;
    }

    char buffer[1024];

    bzero(buffer,sizeof(buffer));
    recv(conn_fd,buffer,1024,0);

    printf("recv: %s\n",buffer);
    bzero(buffer,sizeof(buffer));
    strcpy(buffer,"this is client \n");
    send(conn_fd,buffer,1024,0);
    while(1){
        bzero(buffer,sizeof(buffer));
        scanf("%s",buffer);
        int p = strlen(buffer);
        
        send(conn_fd,buffer,1024,0);

        printf("i have send buffer \n");
    }
    close(conn_fd);
    return 0;
    return 0;
}
