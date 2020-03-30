#include <iostream>
#include <stdio.h>
#include <string.h>
#if 0
int main()
{
	int test = 0;
	//分配512M, 未使用
	char * p = new char [1024*1024*512];
	scanf("%d", &test); //等待输入

	//使用10M
	memset(p, 0, 1024 * 1024 * 10);
	scanf("%d", &test); //等待输入

	//使用50M
	memset(p, 0, 1024 * 1024 * 50);
	scanf("%d", &test); //等待输入
	delete [] p;
	return 0;
}
#else
int main()
{
    int test = 0;
    //20M栈, 未使用
    char p[1024*1024*20];
    scanf("%d", &test);    //等待输入

    //使用10M
    memset(p, 0, 1024 * 1024 * 10);
    scanf("%d", &test);    //等待输入
    return 0;
}
#endif
