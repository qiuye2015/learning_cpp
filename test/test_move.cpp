/*************************************************************************
  > File Name: test_move.cpp
  > Author: fjp
  > Mail: fjp@xxx.com
  > Created Time: Fri 14 Feb 2020 10:04:23 AM CST
 ************************************************************************/

#include <iostream>
#include <utility>
using namespace std;

void func(int &&a){
	cout<<a<<endl;
}
int main(){
	int a = 10;
	func(20);
	//func(a);
	func(move(a));
	return 0;
}
