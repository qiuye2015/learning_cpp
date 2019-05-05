#include<iostream>
#include<fstream>
#include<Mymessage.pb.h>

using namespace std;

int main(){
    Im::Content msg;
    fstream input("./log", ios::in | ios::binary);
    if(!msg.ParseFromIstream(&input)){
        cerr<<"error"<<endl;
        return -1;
    }
    return 0;
}
