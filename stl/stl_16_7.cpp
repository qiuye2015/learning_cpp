#include<iostream>
#include<thread>
#include<chrono>
#include<condition_variable>
#include<mutex>
#include<future>

using namespace std;

recursive_timed_mutex rtm;

void mysleep(short s){
    this_thread::sleep_for(chrono::seconds(s));
}

void job1(){
    long cnt = 6;
    bool b1;
    while(cnt--){
        b1 = rtm.try_lock();
        if(b1){
            cout<<"thread 1 ID:"<<this_thread::get_id()<<endl;
            rtm.unlock();
        }
        mysleep(1);
    }
 }

 void job2(){
    long cnt = 6;
    bool b1;
    while(cnt--){
        b1 = rtm.try_lock_for(chrono::milliseconds(10));;
        if(b1){
            cout<<"thread 2 ID:"<<this_thread::get_id()<<endl;
            rtm.unlock();
        }
        mysleep(1);
    }
 }

  void job3(){
    long cnt = 6;
    bool b1;
    while(cnt--){
        b1 = rtm.try_lock_until(chrono::steady_clock::now()+chrono::milliseconds(10));;
        if(b1){
            cout<<"thread 3 ID:"<<this_thread::get_id()<<endl;
            rtm.unlock();
        }
        mysleep(1);
    }
 }
int main(){ 
    thread t(job1);
    thread t2(job2);
    thread t3(job3);
    t.join();
    t2.join();
    t3.join();
    cout<<"main end"<<endl;
    
    cin.get();

    return 0;
}
