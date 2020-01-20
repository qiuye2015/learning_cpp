/*************************************************************************
    > File Name: String.cpp
    > Author: fjp
    > Mail: fjp@xxx.com
    > Created Time: Mon 13 Jan 2020 03:26:02 PM CST
 ************************************************************************/
#include <iostream>
#include <ostream>
#include <istream>
#include "string.h"

using namespace std;

class String{
private:
	char* m_string;
public:
	//char* m_string;
	String(const char* str=NULL);
	String(const String& other);
	String& operator=(const String& other);
	~String();
	char& operator[](int index);
	bool operator==(const String& other);
	String& operator+=(const String& other);
	String operator+(const String& other);

	ostream& operator<<(ostream& out);
	friend ostream& operator<<(ostream& out,string &s);
	friend istream& operator>>(istream& in, string &s);
	char* m_str();
};
String::String(const char* str){
	if(NULL == str){
		m_string = new char[1];
		m_string = '\0';
	}
	else{
		m_string = new char[strlen(str) + 1];
		strcpy(m_string,str);
	}
};
String::String(const String& other){
	m_string = new char[strlen(other.m_string)+1];
	strcpy(m_string,other.m_string);
}
String& String::operator=(const String& other){
	if(this == &other){
		return *this;
	}
	delete[] m_string;
	m_string = new char[strlen(other.m_string) + 1];
	strcpy(m_string,other.m_string);
	return *this;
}
String::~String(){
	if (NULL != m_string){
		delete[] m_string;
		m_string = NULL;
	}
}
char& String::operator[](int index){
	if(index >= strlen(m_string))
		throw("index error");
	return m_string[index];
}
bool String::operator==(const String& other){
	if(strcmp(m_string,other.m_string) == 0)
		return true;
	return false;
}
String& String::operator+=(const String& other){
	char* tmp = m_string;
	m_string = new char[strlen(m_string) + strlen(other.m_string) + 1];
	strcpy(m_string,tmp);
	delete[] tmp;
	strcat(m_string,other.m_string);
	return *this;
}
String String::operator+(const String& other){
	String tmp;
	tmp.m_string = new char[strlen(m_string) + strlen(other.m_string) + 1];
	strcpy(tmp.m_string,m_string);
	strcat(tmp.m_string,other.m_string);
	return tmp;
}
ostream& String::operator<<(ostream& out){
	out<<"not friend: "<<m_string<<endl;
	return out;
}
ostream& operator<<(ostream& out,String &s){
	out<<"my friend: "<<s.m_str()<<endl;
	//out<<"my friend: "<<s.m_string<<endl;
	return out;
}
istream& operator>>(istream& in,String &s){
	char tmp[1024];
	in.get(tmp,1024);
	s = tmp;
	return in;
}
char* String::m_str(){
	return m_string;
}

int main(){
	String a("hello");//构造函数
	String b(a);//拷贝构造函数
	String c;
	c = a;//赋值函数
	
	a<<cout;// not friend
	cout<<a;
	cout<<"input: ";
	cin>>c;
	cout<<c;
	return 0;
}


