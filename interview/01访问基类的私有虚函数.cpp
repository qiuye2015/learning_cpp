#include<iostream>
using namespace std;

class A{
    virtual void g(){
        cout << "A::g" << endl;
    }
    private:
    virtual void f(){
        cout << "A::f" << endl;
    }
};
//B的布局展开就是：函数g(被B覆写)的地址，函数f(A的f)的地址，函数h的地址
class B :public A
{
    void g(){
        cout << "B::g" << endl;
    }
    virtual void h(){
        cout << "B::h" << endl;
    }
};
typedef void(*Fun)(void);

int main()
{
    B b;
    Fun pFun;
    cout << "虚函数表地址：" << (long*)(&b) << endl;
    cout << "虚函数表 — 第一个函数地址：" << (long*)*(long*)(&b) << endl;
    cout << "虚函数表 — 第二个函数地址：" << (long*)*(long*)(&b)+1 << endl;
    cout << "虚函数表 — 第三个函数地址：" << (long*)*(long*)(&b)+2 << endl;
    for (int i = 0; i < 3; i++){
        pFun = (Fun)*((long*)*(long*)(&b) + i);//访问基类的私有虚函数
        pFun();
    }
    return 0;
}
