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
    int server_fd,accept_fd;//监听socket，数据传输socket
    struct sockaddr_in my_addr;//本机地址
    struct sockaddr_in their_addr;//客户地址信息
    unsigned int listen_num=10;

    if((server_fd = socket(AF_INET,SOCK_STREAM,0))== -1){
        perror("socket error\n");
        return -1;
    }
    printf("socket ok \n");

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEFAULT_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero),0);

    if( bind(server_fd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr))== -1){
        perror("bind error\n");
        return -2;
    }
    printf("bind ok \n");

    if(listen(server_fd,listen_num)==-1){
        perror("listen error \n");
        return -3;
    }
    printf("listen ok \n");
    
    fd_set client_fdset; //监控文件描述符集合
    int maxsock; //监控文件描述符中最大的文件号
    struct timeval tv;//超时返回时间

    int client_sockfd[5];//存放活动的sockfd
    bzero((void*)client_sockfd,sizeof(client_sockfd));

    int conn_amount = 0;//用来记录描述符数量
    maxsock = server_fd;

    char buffer[1024];
    int ret=0;
    
    while(1){
        FD_ZERO(&client_fdset);//初始化文件描述符集合
        FD_SET(server_fd,&client_fdset);//加入服务器描述符
        //设置超时时间
        tv.tv_sec = 30;//30s
        tv.tv_usec = 0;
        //把活动的句柄加入到文件描述符中
        for(int i=0;i<5;++i){
            if(client_sockfd[i] !=0){
                FD_SET(client_sockfd[i],&client_fdset);
            }
        }

        ret = select(maxsock+1,&client_fdset,NULL,NULL,&tv);

        if(ret<0){
            perror("select error~\n");
            break;
        }
        else if(ret==0){
            printf("timeout \n");
            continue;
        }

        //轮询各个文件描述符
        for(int i =0;i<conn_amount;++i){
            //检查client_sockfd是否可读可写，>0可读写
            if(FD_ISSET(client_sockfd[i],&client_fdset)){
                printf("start recv from client [%d]:\n",i);
                
                ret =recv(client_sockfd[i],buffer,1024,0);
                if(ret<=0){
                    printf("client [%d] close \n",i);
                    close(client_sockfd[i]);
                    FD_CLR(client_sockfd[i],&client_fdset);
                    client_sockfd[i]=0;
                }else{
                    printf("recv from client [%d] : %s\n",i,buffer);
                }
            }
        }
        //检查是否有新的连接，如果有，接收连接，加入到client_sockfd中
        if(FD_ISSET(server_fd,&client_fdset)){
            //接受连接
            struct sockaddr_in client_addr;
            size_t size = sizeof(struct sockaddr_in);
            int sock_client = accept(server_fd,(struct sockaddr*)(&client_addr),(unsigned int *)&size);
            if(sock_client <0){
                perror("accept error\n");
                continue;
            }
            //把连接加入到文件描述符集合中
            if(conn_amount<5){
                client_sockfd[conn_amount++] = sock_client;
                bzero(buffer,1024);
                
                strcpy(buffer,"this is server,welcome!\n");
                send(sock_client,buffer,1024,0);
                printf("new connection client [%d] %s:%d\n",conn_amount,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
                bzero(buffer,sizeof(buffer));

                ret = recv(sock_client,buffer,1024,0);
                if(ret<0){
                    perror("recv error \n");
                    close(server_fd);
                    return -1;
                }
                printf("recv : %s\n",buffer);
                if(sock_client > maxsock){
                    maxsock = sock_client;
                }
                else{
                    printf("max connections !!! quit\n");
                }
            }
        }
    }

    for(int i=0;i<5;++i){
        if(client_sockfd[i] !=0){
            close(client_sockfd[i]);
        }
    }
    close(server_fd);

    return 0;
}
