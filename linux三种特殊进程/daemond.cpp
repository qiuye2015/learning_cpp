/*************************************************************************
  > File Name: daemond.cpp
  > Author: fjp
  > Mail: fjp@dangdang.com 
  > Created Time: Wed 20 Nov 2019 10:11:00 AM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

/* 创建守护进程:
 * 1. fork 创建子进程
 * 2. setsid 设置新会话
 * 3. 设置文件掩码&关闭文件描述符&改变当前工作目录
 * 第一次fork的作用：让shell认为本条命令已经终止，不用挂在终端输入上。
 * 还有一个作用是为后面setsid服务。setsid的调用者不能是进程组组长(group leader)，此时父进程是进程组组长
 * 第二次的作用是：防止进程再次打开一个控制终端。
 * 因为打开一个控制终端的前台条件是该进程必须是会话组长。再fork一次，子进程ID != sid（sid是进程父进程的sid）。
 * 所以也无法打开新的控制终端。
 * 因此，第二次fork不是必须的。但是为了安全起见，调用两次是比较保险的。
 */
void mydaemond(){

    pid_t pid;
    if((pid=fork())<0){
        perror("fork error\n");
    }
    else if(pid>0){ // 父进程终止
        exit(0);
    }
    // 子进程创建新会话
    if(setsid() <0){
        perror("setsid error\n");
    }
    #if 0
    if((pid=fork())<0){
        perror("fork2 error\n");
    }
    else if(pid>0){ // 父进程终止
        exit(0);
    }
    #endif
    chdir("/");
    umask(0002);
    // 关闭文件描述符
    close(STDIN_FILENO);
    int fd0 = open("/dev/null",O_RDWR);//fd = 0(STDIN_FILENO)
	//使newfd成为oldfd的副本 int dup2(int oldfd, int newfd);
    dup2(fd0,STDOUT_FILENO);
    dup2(fd0,STDERR_FILENO);
}
int main(){
    mydaemond();
    //while(1);
    int fd = open("/home/fjp/daemonTest.log",O_RDWR | O_CREAT,0644);
    time_t t;
    for(int i=0;i<3;++i){
        t = time(0);
        char* buf = asctime(localtime(&t));
        write(fd, buf, strlen(buf));
        char tmp[2];
        sprintf(tmp,"%d\n",fd);
        write(fd, tmp,strlen(tmp));
        sleep(10);
    }
    close(fd);
    return 0;
}
