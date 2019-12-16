/*************************************************************************
    > File Name: orphan_process.cpp
    > Author: fjp
    > Created Time: Mon 02 Dec 2019 02:31:57 PM CST
 ************************************************************************/

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

int main(){
    int pid;
    // 防止产生僵尸进程
    //signal(SIGCHLD,SIG_IGN);
    printf("before fork pid:%d\n",getpid());

    pid = fork();
    if (pid <0){
        perror("fork error \n");
    }
    else if (pid > 0){
        printf("parent pid = %d\n",getpid());
        sleep(60);
        printf("end parent pid = %d\n",getpid());
    }
    else{
        printf("child pid = %d,parent %d\n",getpid(),getppid());
        sleep(20);
        printf("end child pid = %d,parent %d\n",getpid(),getppid());
    }
    printf("fork after...\n");
    return 0;

}
