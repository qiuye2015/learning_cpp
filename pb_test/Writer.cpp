#include<iostream>
#include<fstream>
#include<Mymessage.pb.h>
using namespace std;

int main(){
    Im::Content msg;
    msg.set_id(110);
    msg.set_str("test");
    fstream output("./log",ios::out | ios::trunc | ios::binary);
    if(!msg.SerializeToOstream(&output)){
        cout<<"error"<<endl;
        return -1;
    }
    return 0;
}
