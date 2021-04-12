/*************************************************************************
  > File Name: 01_simple_reactor.c
  > Author: fjp
  > Mail: fjp@xxx.com
  > Created Time: Mon 12 Apr 2021 04:48:28 PM CST
  > 反应堆简单版
 ************************************************************************/

#include <stdio.h>
#include "wrap.h"
#include <sys/epoll.h>


#define _BUF_LEN_ 1024
#define _EVENT_SIZE_ 1024

//全局epoll树的根
int g_epfd = 0;

typedef struct {
	int fd;
	int events;
	void (*call_back)(int fd, int events, void *arg);
	//void *arg;
	char buf[_BUF_LEN_];
	int buflen;
	//int epfd;//epoll树的根
}xevent;

xevent myevents[_EVENT_SIZE_+1];

//添加事件
void event_add(int fd,int events,void (*call_back)(int,int,void*),xevent *xev){
	xev->fd = fd;
	xev->events = events;
	xev->call_back = call_back;

	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = xev;//核心思想
	epoll_ctl(g_epfd, EPOLL_CTL_ADD, fd, &ev);//上树
}
//修改事件
void event_set(int fd,int events,void (*call_back)(int,int,void*),xevent *xev){
	xev->fd = fd;
	xev->events = events;
	xev->call_back = call_back;

	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = xev;//核心思想
	epoll_ctl(g_epfd, EPOLL_CTL_MOD, fd, &ev);//修改
}
//删除事件
void event_del(int fd,int events,xevent *xev){
	xev->fd = 0;
	xev->events = 0;
	xev->call_back = NULL;
	memset(xev->buf,0x00,sizeof(xev->buf));
	xev->buflen = 0;

	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = NULL;
	epoll_ctl(g_epfd, EPOLL_CTL_DEL, fd, &ev);//下树
}

void send_data(int fd,int events,void *arg);

//读数据
void read_data(int fd,int events,void *arg){
	printf("fd =%d, call %s\n",fd,__FUNCTION__);
	xevent *xev = arg;
	xev->buflen = Read(fd,xev->buf,sizeof(xev->buf));
	if (xev->buflen > 0) {//读到数据
		event_set(fd, EPOLLOUT,send_data,xev);
		printf("read & send\n\n");
	}else if (xev->buflen == 0) {//对方关闭连接
		Close(fd);
		printf("read & close\n\n");
		event_del(fd,EPOLLIN,xev);
	}
}

//发送数据
void send_data(int fd,int events,void *arg){
	printf("fd =%d, call %s\n\n",fd,__FUNCTION__);
	xevent *xev =arg;
	Write(fd,xev->buf,xev->buflen);
	event_set(fd,EPOLLIN,read_data,xev);
}

void initAccept(int fd,int events,void *arg){
	printf("fd =%d, call %s\n",fd,__FUNCTION__);
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int cfd = Accept(fd,(struct sockaddr*)&addr,&len);
	char ip[16]="";
	printf("new client fd=%d ip=%s port=%d\n\n",fd,
			inet_ntop(AF_INET,&addr.sin_addr.s_addr,ip,16),
			ntohs(addr.sin_port));
	int i;
	//查找myevents数组中可用的位置
	for(i=0;i<_EVENT_SIZE_;i++){
		if(myevents[i].fd == 0){
			break;
		}
	}
	event_add(cfd,EPOLLIN,read_data,&myevents[i]);

}

int main(int argc, char **argv) {
	//创建socket
	int lfd = tcp4bind(6666,NULL);
	//监听
	Listen(lfd,128);

	//创建epoll树根节点
	g_epfd = epoll_create(1024);
	printf("lfd === %d, gepfd === %d\n\n",lfd, g_epfd);
	struct epoll_event events_vec[1024];
	//添加最初始事件，将侦听的描述符上树
	event_add(lfd,EPOLLIN,initAccept,&myevents[_EVENT_SIZE_]);
	while(1){
		int nready = epoll_wait(g_epfd,events_vec,1024,-1);
		if(nready<0) {//调用失败
			perr_exit("epoll_wait error");
		}else if(nready>0){
			//调用epoll_wait成功,返回有事件发生的文件描述符的个数
			int i;
			for(i=0;i<nready;i++){
				xevent *xev = events_vec[i].data.ptr;//核心1
				//取ptr指向结构体地址
				printf("epoll_wait fd =%d,events = %d\n",xev->fd,xev->events);
				if(xev->events & events_vec[i].events){
					//调用事件对应的回调
					xev->call_back(xev->fd,xev->events,xev);//核心2
				}
			}
		}
	}
	//关闭监听文件描述符
	Close(lfd);
	return 0;
}

