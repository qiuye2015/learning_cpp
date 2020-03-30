/*************************************************************************
  > File Name: test_exit.c
  > Author: fjp
  > Mail: fjp@xxx.com
  > Created Time: Tue 11 Feb 2020 05:02:24 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	printf("hello\n");
	printf("  hi");
	//exit(0);
	_exit(0);
	return 0;
}

