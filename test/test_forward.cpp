/*************************************************************************
  > File Name: test_forward.cpp
  > Author: fjp
  > Mail: fjp@xxx.com
  > Created Time: Fri 14 Feb 2020 10:11:12 AM CST
 ************************************************************************/

#include <iostream>
#include <utility>
using namespace std;

void func(int &a){
	cout<<"l value"<<a<<endl;
}
void func(int &&a){
	cout<<"r value"<<a<<endl;
}

template<class T>
void wrapper(T &&args){
	func(args);
}
template<class T>
void wrapper2(T &&args){
	func(forward<T>(args));
}
int main(){
	int a = 10;
	wrapper(a);
	wrapper(20);
	wrapper2(a);
	wrapper2(20);
	return 0;
}
