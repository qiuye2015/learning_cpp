/*************************************************************************
  > File Name: test.cpp
  > Author: fjp
  > Mail: fjp@xxx.com
  > Created Time: Sun Jan 10 22:20:49 2021
 ************************************************************************/

#include <iostream>
#include "ftest.h"
using namespace std;
int add(int a,int b){
	return a+b;
}
TEST(add,7){
	EXCEPT_EQ(add(3,4),8);
	EXCEPT_EQ(add(3,4),7);
}
int main(){
	
	printf(RED("starting\n"));
	RUN_ALL_TEST();
	return 0;
}
