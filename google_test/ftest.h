/*************************************************************************
  > File Name: ftest.h
  > Author: fjp
  > Mail: fjp@xxx.com
  > Created Time: Sun Jan 10 21:13:49 2021
 ************************************************************************/

#include <iostream>
using namespace std;
#ifndef _FTEST_H
#define _FTEST_H

#ifdef __cplusplus
extern "C"
{
#endif

#define EXCEPT(a,comp,b) {\
	if (!((a) comp (b))) {\
		printf("error\n");\
	}\
}
#define EXCEPT_EQ(a,b) EXCEPT(a,==,b)
#define EXCEPT_NE(a,b) EXCEPT(a,!=,b)

#define COLOR(msg,code) "\033[0;1;" #code "m" msg"\033[0m"
#define RED(msg) COLOR(msg,31)

struct {
	void (*func)();
	const char * func_name;
}func_addr[100];

int func_cnt=0;
int RUN_ALL_TEST(){
	for(int i=0;i<func_cnt;i++) {
		func_addr[i].func();
	}
	return 0;
}
void add_test_func(void (*func)(),const char* str){
	func_addr[func_cnt].func=func;
	func_addr[func_cnt].func_name=str;
	func_cnt++;
}
#define TEST(a,b) \
void fjp_##a##_##b();\
__attribute__((constructor))\
void reg_##a##_##b(){\
	add_test_func(fjp_##a##_##b,#a "." #b);\
}\
void fjp_##a##_##b()

#ifdef __cplusplus
}
#endif

#endif /* _FTEST_H */


