#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <boost/archive/text_oarchive.hpp>//文本格式输出存档
#include <boost/archive/text_iarchive.hpp>//文本格式输入存档

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace std;

class Test
{
public:
	friend class boost::serialization::access;

	Test(bool b, char ch, int i, double d, string str)
		: m_bool(b), m_char(ch), m_int(i), m_double(d), m_str(str)
	{
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar& m_bool;
		ar& m_char;
		ar& m_int;
		ar& m_double;
		ar& m_str;
	}

//private:
	bool m_bool;
	char m_char;
	int m_int;
	double m_double;
	string m_str;
};

int main()
{
	Test test(true, 'm', 50, 17.89, "outTest");

	stringstream binary_sstream;
	stringstream text_sstream;

	long long begin, end;

	int size;

	//使用二进制的方式序列化
	boost::archive::text_oarchive text_oa(text_sstream);//文本方式
	boost::archive::binary_oarchive binary_oa(binary_sstream);//二进制方式

	begin = time(NULL);
	for(int i = 0; i < 1000000; ++i)
	{
		text_oa << test;
	}
	end = time(NULL);

	size = text_sstream.tellp() / (1024 * 1024);

	cout << "text serialization seconds: " << end - begin << ", space: " << size << endl;

	begin = time(NULL);
	for(int i = 0; i < 1000000; ++i)
	{
		binary_oa << test;//将对象test的序列化数据以二进制存储形式写入内存
	}
	end = time(NULL);

	//以MB为单位
	size = binary_sstream.tellp() / (1024 * 1024);

	cout << "binary serialization seconds: " << end - begin << ", space: " << size << endl;
//--------------------------------------------------------------------------
	Test iTest(false, 'n', 100, 0.00, "in");
	ofstream ofs("serial.txt"); //输出文件流
	boost::archive::binary_oarchive oa(ofs);
	oa & test;   //序列化到输出存档
	ofs.close();

	cout<<iTest.m_str<<endl;
	ifstream ifs("serial.txt");//文件输入流
	boost::archive::binary_iarchive ia(ifs);
	ia & iTest;
	ifs.close();
	cout<<iTest.m_str<<endl;
	return 0;
};
