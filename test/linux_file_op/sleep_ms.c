/*************************************************************************
  > File Name: sleep_ms.c
  > Author: fjp
  > Created Time: Thu 19 Dec 2019 11:51:28 AM CST
 ************************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#if 0
void sleep_ms(unsigned int ms){
	struct timeval tval;
	tval.tv_sec = ms/1000;
	tval.tv_usec = ms*1000%1000000;
	select(0,NULL,NULL,NULL,&tval);
}
#else
// 精度高
void sleep_ms(unsigned int ms){
	int ret;
	struct timespec req,rem;
	req.tv_sec = ms/1000;
	req.tv_nsec = (ms%1000)*1000000;
	ret = nanosleep(&req,&rem);
	if(ret<0){
		perror("nanosleep error\n");
	}
}
#endif
int main(){
	unsigned int time = 500;
	for(;;){
		sleep_ms(time);
		printf("sleep_ms %d\n",time);
	}
}
