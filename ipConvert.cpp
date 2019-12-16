/*************************************************************************
    > File Name: main.cpp
    > Author: fjp
    > Created Time: Thu 14 Nov 2019 10:17:36 AM CST
 ************************************************************************/

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

const char *ddip_int2ip(const int ip_int,                                 
                        char *dest, const size_t dest_size)                     
{                                                                               
  struct in_addr ina;                                                           
  ina.s_addr = ntohl(ip_int);                                                   
  unsigned char *ucp = (unsigned char *)&ina;                                   
  snprintf(dest, dest_size, "%d.%d.%d.%d",                                      
           ucp[0] & 0xff,                                                       
           ucp[1] & 0xff,                                                       
           ucp[2] & 0xff,                                                       
           ucp[3] & 0xff);                                                      
  return dest;                                                                  
}

uint32_t ddip_ip2int(const char *ip){
	return ntohl(inet_addr(ip));
}
int main(){
	unsigned int_ip;
	char outIP[16];
	int flag = 0;
	cout<<"Please Choose 1(int2str),0(str2int)default"<<endl;
	cin>>flag;
	while(true){
		if(flag){
			cout<<"Please input int IP:";
			cin>>int_ip;
			ddip_int2ip(int_ip,outIP,16);
			cout<<outIP<<endl;
			
		}else{
			cout<<"Please input str IP:";
			cin>>outIP;
			cout<<ddip_ip2int(outIP)<<endl;
		}
	}
}
