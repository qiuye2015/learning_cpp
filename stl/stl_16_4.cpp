#include<iostream>
#include<thread>
#include<chrono>
#include<condition_variable>
#include<mutex>
#include<future>

using namespace std;

mutex m,m2;
long shareddata[11];
void thread_proc(){
    int index =0;
    for(index;index<5;index++){
        m.try_lock();
        shareddata[2*index] = 2*index;
        cout<<"thread 1--shareddate:"<< shareddata[2*index]<<endl;
        m.unlock();
        chrono::milliseconds sd(1000);
        this_thread::sleep_for(sd);
    }
    cout<<"thread 1,exit"<<endl;
}
void thread_proc2(){
    int index2 =0;
    for(index2;index2<5;index2++){
        m.try_lock();
        shareddata[2*index2+1] = 2*index2+1;
        cout<<"thread 2--shareddate:"<< shareddata[2*index2+1]<<endl;
        m.unlock();
        chrono::milliseconds sd(10);
        this_thread::sleep_for(sd);
    }
    cout<<"thread 2,exit"<<endl;
}


int main(){ 
	cout<<"main start"<<endl;
    thread t(thread_proc);
    thread t2(thread_proc2);
    t.detach();
    t2.join();
    cout<<"main end"<<endl;
    
    cin.get();

    return 0;
}
