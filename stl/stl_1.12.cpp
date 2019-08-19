#include<iostream>
#include<cstdio>

using namespace std;

int main(){
   cout<<TMP_MAX<<" " <<L_tmpnam<<endl;
   char pszName[L_tmpnam]={'\0'};
   for(int i=0;i<10;i++){
    tmpnam(pszName);
    cout<<pszName<<endl;
   }
    return 0;
}
