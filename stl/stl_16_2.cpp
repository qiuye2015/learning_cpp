#include<iostream>
#include<thread>

using namespace std;
void mysleep(std::chrono::microseconds us){
	auto s = std::chrono::high_resolution_clock::now();
	auto e = s + us;
	do{
		std::this_thread::yield();
	}while(std::chrono::high_resolution_clock::now() < e);
}
int main(){ 
	auto start = std::chrono::high_resolution_clock::now();
	mysleep(std::chrono::microseconds(10000));
	auto elapsed = std::chrono::high_resolution_clock::now()-start;
	cout<< std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()<<endl;
	cin.get();

	return 0;
}
