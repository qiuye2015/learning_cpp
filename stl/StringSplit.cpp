#include<iostream>
#include<vector>
#include<string>

using namespace std;

vector<string> StringSplit(const string& sData, const string& sDelim){
    vector<string> vItems;
    vItems.clear();
    string::size_type bpos = 0;//开始位置
    string::size_type epos = 0;//结束位置
    string::size_type nlen = sDelim.size();

    while((epos = sData.find(sDelim, epos)) != string::npos){
        vItems.push_back(sData.substr(bpos,epos-bpos));
        epos +=nlen;//结束位置添加分隔符大小
        bpos = epos;
    }
    vItems.push_back(sData.substr(bpos,sData.size()-bpos));
    return vItems;
}

int main(){
    string tmp("asdf&qwer");
    vector<string> res = StringSplit(tmp,"&");
    for(int i=0;i<res.size();++i){
        cout<<res[i]<<" ";
    }
    return 0;

}
