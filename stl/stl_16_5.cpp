#include<iostream>
#include<thread>
#include<chrono>
#include<condition_variable>
#include<mutex>
#include<future>

using namespace std;

//mutex m;
recursive_mutex m;
long shareddata[11];

void mysleep(){
	chrono::milliseconds sd(100);
	this_thread::sleep_for(sd);
}
void thread_proc(){
    int index =0;
    for(index;index<5;index++){
        m.try_lock();
        shareddata[2*index] = 1;
        cout<<"thread 1--shareddate:"<< shareddata[2*index+1]<<endl;
        m.unlock();
		mysleep();
    }
    cout<<"thread 1,exit"<<endl;
}
void thread_proc2(){
    int index2 =0;
    for(index2;index2<5;index2++){
        m.try_lock();
        shareddata[2*index2+1] = 2;
        cout<<"thread 2--shareddate:"<< shareddata[2*index2+1]<<endl;
        m.unlock();
		mysleep();
    }
    cout<<"thread 2,exit"<<endl;
}


int main(){ 
	cout<<"main start"<<endl;
    thread t(thread_proc);
    thread t2(thread_proc2);
    t.join();
    t2.join();
    cout<<"main end"<<endl;
    
    cin.get();

    return 0;
}
