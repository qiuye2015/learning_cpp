/*************************************************************************
    > File Name: timer.cpp
    > Author: fjp
    > Created Time: Fri 13 Dec 2019 02:37:18 PM CST
 ************************************************************************/

#include <boost/timer.hpp>
#include <boost/progress.hpp>
#include <iostream>
using namespace boost;
using namespace std;

int main()
{
     boost::progress_timer     t;  //声明一个计时器,开始计时
    //dosomething
     for(int i=0;i<100;i++)
     {
         cout<<"a";
     }
}
