#include<sys/time.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<assert.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<sys/select.h>

int main(){
    int keyboard_fd;
    int ret, i;
    char c;
    fd_set read_fd;
    struct timeval timeout;
    keyboard_fd = open("/dev/tty",O_RDONLY | O_NONBLOCK);
    assert(keyboard_fd>0);
    while(1){
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        FD_ZERO(&read_fd);
        FD_SET(keyboard_fd,&read_fd);
        ret = select(keyboard_fd+1,&read_fd, NULL,NULL,&timeout);

        if(ret ==-1){
            perror("select error\n");
        }
        else if(ret){
            if(FD_ISSET(keyboard_fd,&read_fd)){
                i = read(keyboard_fd,&c,1);
                if('\n'==c){
                    continue;
                }
                printf("The input is %c\n",c);
                if('q'==c){
                    break;
                }
            }
        }
        else if(ret==0){
            printf("time out\n");
        }
    }
    return 0;
}
