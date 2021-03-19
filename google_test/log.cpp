/*************************************************************************
  > File Name: log.cpp
  > Author: fjp
  > Mail: fjp@xxx.com
  > Created Time: Sun Jan 10 19:39:04 2021
 ************************************************************************/

#include <iostream>
/*#include <queue>
#include <stack>
#include <algorithm>
#include <map>
#include <set>
#include <vector>*/
using namespace std;

#define  LOG(frm,args...) do{ \
	printf("\033[0;33m[%s:%s:%d] \033[0m",__FILE__,__func__,__LINE__);\
	printf(frm,##args);\
	printf("\n");\
}while(0);
int main(){
	printf("hello world\n");
	LOG("hello world\n");
	return 0;
}
