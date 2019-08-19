#include<iostream>
#include<thread>
#include<chrono>
#include<condition_variable>
#include<mutex>
#include<future>

using namespace std;

bool cont = 0;
void thread_entry1(){
    long a=10000;
    int sec=0;
    cout<<"waiting for 10 sec..."<<endl;
    while(a--){
        chrono::milliseconds dura(1);
        this_thread::sleep_for(dura);
    }
    cout<<"waiting for 8 sec..."<<endl;
    while(sec<=8){
        sec++;
        this_thread::sleep_until(chrono::system_clock::now()+chrono::seconds(sec));
        cout<<"-";
    }
    cout<<endl;
    cout<<"thread(1) id: "<<this_thread::get_id()<<endl;
    cont=1;
}
void thread_entry2(){
    cout<<"Thread(2) id:"<<this_thread::get_id()<<endl;
    cout<<"thread(2) exit..."<<endl;
}

int main(){ 
  
    chrono::milliseconds dura(2000);
    this_thread::sleep_for(dura);
    cout<<"waiting 2000 ms\n";
    thread t(thread_entry1);
    thread t2(thread_entry2);
    cout<<"thread(1) origin id: "<<t.get_id()<<endl;
    cout<<"thread(2) origin id: "<<t2.get_id()<<endl;
    t.swap(t2);
    cout<<"thread(1) swap id: "<<t.get_id()<<endl;
    cout<<"thread(2) swap id: "<<t2.get_id()<<endl;
    t2.swap(t);
    cout<<"thread(1) swap secondly id: "<<t.get_id()<<endl;
    cout<<"thread(2) swap secondly id: "<<t2.get_id()<<endl;
    if(t.joinable())
        t.detach();
    if(t2.joinable())
        t2.join();
    thread::native_handle_type hd = t.native_handle();
    cout<<"native handle:" <<hd <<endl;
    unsigned int tc = t.hardware_concurrency();
    cout<<"hardware_concurrenc"<<tc<<endl;
    cin.get();

    return 0;
}
