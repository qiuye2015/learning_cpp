/*************************************************************************
  > File Name: test_erase.cpp
  > Author: fjp
  > Mail: fjp@xxx.com
  > Created Time: Fri 14 Feb 2020 04:42:33 PM CST
 ************************************************************************/

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
void printVec(const vector<int> &nums){
	for(auto&& num: nums){
		cout<<num<<" ";
	}
	cout<<endl;
}

int main(){
	std::vector<int> c { 1,2,3,4,5,6,7 };
	int x = 5;
	c.erase(std::remove_if(c.begin(), c.end(), [x](int n) { return n < x; } ), c.end());
	printVec(c);
	return 0;
}
