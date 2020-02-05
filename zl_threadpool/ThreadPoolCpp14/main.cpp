#include <iostream>
#include <unistd.h> //sleep
#include "ThreadPool.h"

void print(int n){
	std::cout<<n<<std::endl;
}

int main()
{
    std::cout << "Hello world" << std::endl;
	ThreadPool tp(10);
	for(int i = 0; i < 20; ++i){
		std::cout<<"------"<<i<<std::endl;
		tp.execute(std::bind(print,i));
	}
	sleep(2);
    return 0;
}

