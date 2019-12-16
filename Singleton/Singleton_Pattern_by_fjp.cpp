/*************************************************************************
    > File Name: main.cpp
    > Author: fjp
    > Mail: fjp@dangdang.com 
    > Created Time: Wed 04 Dec 2019 05:31:26 PM CST
 ************************************************************************/

#include <iostream>
using namespace std;
#if 1
// 饿汉式
//线程安全
class Singleton{
private:
    //static Singleton m;
    static Singleton* m;
private:
    Singleton(){
        cout<<"构造函数"<<endl;
    }
    ~Singleton(){
        cout<<"析构函数"<<endl;
    }
    Singleton(const Singleton& other);
    Singleton& operator=(const Singleton& other);
public:
    static Singleton* getInstance(){
        //return &m;
        return m;
    }
    class garbage{
    public:
        garbage(){
            cout<<"garbage"<<endl;
        }
        ~garbage(){
            cout<<"~garbage"<<endl;
            if(m!=NULL){
                delete m;
                m = NULL;
            }
        }
    };

private:
    static garbage m_g;

};
//Singleton Singleton::m;
Singleton* Singleton::m = new Singleton();
Singleton::garbage Singleton::m_g;

#else
//懒汉式
class Singleton{
private:
    Singleton(){
        cout<<"构造函数"<<endl;
    }
    Singleton(const Singleton& other);
    Singleton& operator=(const Singleton& other);
    ~Singleton(){
        cout<<"析构函数"<<endl;
    }
    static Singleton* m;
public:
    static Singleton* getInstance(){
      //  static Singleton m;
      //  return &m;
      if(m == NULL){
        m = new Singleton();
      }
      return m;
    }
};
Singleton* Singleton::m = NULL;

#endif


int main(){
    Singleton* m1 = Singleton::getInstance();
    Singleton* m2 = Singleton::getInstance();
    Singleton* m3 = Singleton::getInstance();
    return 0;
}
