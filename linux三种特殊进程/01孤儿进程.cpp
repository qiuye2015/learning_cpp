#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <signal.h>

int main(void)
{
        pid_t pid ;
        signal(SIGCHLD,SIG_IGN);
        printf("before fork pid:%d\n",getpid());
        int abc = 10;
        pid = fork();
        if(pid == -1)
        {
                perror("tile");
                return -1;
        }
        if(pid > 0)           //父进程先退出
        {
                abc++;
                printf("parent:pid:%d \n",getpid());
                printf("parent-abc:%d \n",abc);
                sleep(5);
        }
        else if(pid == 0){  //值进程后退出,被托付给init进程
                abc++;
                printf("child:%d,parent: %d\n",getpid(),getppid());
                printf("child-abc:%d",abc);
                sleep(100);
        }
        printf("fork after...\n");
}
