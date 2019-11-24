#include <iostream>
#include "Singleton.h"

using namespace std;

class MyClass {
public:
	MyClass(const string& strData) : m_strData(strData) {
		cout << m_strData.data() << endl;
	};
	~MyClass() {
		cout << "destory" << endl;
	};

private:
	string m_strData;
};

auto main()->int { //等同于入口函数int main()
	auto pClass = Singleton<MyClass>::GetInstance("create");
	auto pClass2 = Singleton<MyClass>::GetInstance("create2");

	//Singleton<MyClass>::DesInstance();

	return 1;
}
