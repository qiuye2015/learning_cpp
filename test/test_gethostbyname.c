/*************************************************************************
  > File Name: test_gethostbyname.c
  > Author: fjp
  > Mail: fjp@xxx.com
  > Created Time: Tue 25 Feb 2020 05:59:33 PM CST
 ************************************************************************/

#include <stdio.h>
#include <netdb.h>

int main(){

	struct hostent *host = gethostbyname("www.dangdang.com");
	if (!host){
		puts("Get IP address error!");
		exit(0);
	}

}
