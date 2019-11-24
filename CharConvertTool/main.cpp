#include<iostream>
#include<string>
#include "encode_convert.h"
using namespace std;

char Char2Int(char ch)
{
   if(ch >= '0' && ch <= '9') return (char)(ch-'0');
   if(ch >= 'a' && ch <= 'f') return (char)(ch - 'a' + 10);
   if(ch >= 'A' && ch <= 'F') return (char)(ch - 'A' + 10);
   return -1;
}

char Str2Bin(char* str)
{
   char tempWord[2];
   char chn;
   tempWord[0] = Char2Int(str[0]);
   tempWord[1] = Char2Int(str[1]);
   chn = (tempWord[0] << 4) | tempWord[1];
   return chn;
}
//url中的中文解析 为GBK
string UrlDecode(const string& src)
{
    string outPut = ""; 
    char tmp[2];
    int i = 0, idx = 0, ndx, len = src.length();
    while(i < len)
    {   
       if(src[i] == '%')
       {   
          tmp[0] = src[i + 1]; 
          tmp[1] = src[i + 2]; 
          outPut += Str2Bin(tmp);
          i = i+3;
       }   
       else if(src[i] == '+')
       {   
           outPut += " ";
           i++;
       }   
       else
       {   
           outPut += src[i];
           i++;
       }   
    }   
    return outPut;
}


int main(){
//    string a = "C+++Primer%D6%D0%CE%C4%B0%E6+%B5%DA5%B0%E6+%B1%E0%B3%CC%D7%D4%D1%A7%3Cbr%3E%3Cbr%3EC++%B1%E0%B3%CC%B4%D3%C8%EB%C3%C5%B5%BD%BE%AB%CD%A8";
    string a;
    cout<<"input:";
    cin>>a;
    string url = UrlDecode(a);
    cout<<"GBK URL:\t"<<url<<endl;
    string utf8_str;
    _GBK_to_UTF8_(url,utf8_str);
    cout<<"UTF-8 URL:\t"<<utf8_str<<endl;
}
