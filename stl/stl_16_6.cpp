#include<iostream>
#include<thread>
#include<chrono>
#include<condition_variable>
#include<mutex>
#include<future>

using namespace std;

timed_mutex tmut;
void job1(){
    bool b1=0;
    int count = 5;
    while(count--){
        b1=tmut.try_lock_for(chrono::milliseconds(9));
        cout<<"thread 1 : "<<count<<endl;
        if(b1){
            tmut.unlock();
        }
    }
    cout<<"thead1 exit"<<endl;
}
void job2(){
    bool b1=0;
    int count = 5;
    while(count--){
        b1=tmut.try_lock_for(chrono::milliseconds(9));
        cout<<"thread 2 : "<<count<<endl;
        if(b1){
            tmut.unlock();
        }
    }
    cout<<"thead2 exit"<<endl;
}
int main(){ 
    thread t(job1);
    thread t2(job2);
    t.join();
    t2.join();
    cout<<"main end"<<endl;
    
    cin.get();

    return 0;
}
