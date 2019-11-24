#include<iostream>
#include<thread>
#include<chrono>
#include<condition_variable>
#include<mutex>
#include<future>

using namespace std;

mutex mt;

void mysleep(int ms){
    chrono::seconds m(ms);
    this_thread::sleep_for(m);
}

void job1(){
    long cnt = 3;
    while(cnt--){
        lock_guard<mutex> lg(mt);
        cout<<"thread 1 ID : "<<this_thread::get_id()<<endl;
        mysleep(1);
    }
}
void job2(){
    long cnt = 3;
    mt.lock();

    while(cnt--){
        lock_guard<mutex> lg(mt,adopt_lock);
        cout<<"thread 2 ID : "<<this_thread::get_id()<<endl;
        mysleep(1);
    }
}
int main(){ 
  
    thread t1(job1);
    thread t2(job2);
    t1.join();
    t2.join();
    cout<<"main end"<<endl;
    
    cin.get();

    return 0;
}
