#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

//清单1.将字符串保存到文本归档文件中
void save()
{
    std::ofstream file("archive.txt");
    boost::archive::text_oarchive oa(file);
    std::string s = "Hello world\n";
    oa << s;  // oa & s; 清单3.使用&运算符执行“转储-恢复”操作
}

// 清单2.将字符串的内容加载到文本文件中
void load()
{
    std::ifstream file("archive.txt");
    boost::archive::text_iarchive ia(file);
    std::string s;
    //ia >> s;  
    ia & s; //清单3.使用&运算符执行“转储-恢复”操作
    std::cout << s << std::endl;
}

int main()
{
    save();
    load();
    getchar();
}
