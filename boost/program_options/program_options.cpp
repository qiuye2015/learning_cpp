//linux系统下,编译选项需加上 -lboost_program_options
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

int main(int argc, char const *argv[])
{
	//步骤一: 构造选项描述器和选项存储器

	//选项描述器,其参数为该描述器的名字
	bpo::options_description opts("all options");

	//选项存储器,继承自map容器
	bpo::variables_map vm;

	//步骤二: 为选项描述器增加选项
	//其参数依次为: key, value的类型，该选项的描述
	opts.add_options()
		("filename", bpo::value<std::string>(), "the file name which want to be found")
		("help", "this is a program to find a specified file");
	//步骤三: 先对命令行输入的参数做解析,而后将其存入选项存储器
	//如果输入了未定义的选项，程序会抛出异常，所以对解析代码要用try-catch块包围
	try{
		//parse_command_line()对输入的选项做解析
		//store()将解析后的结果存入选项存储器
		bpo::store(bpo::parse_command_line(argc, argv, opts), vm);
	}
	catch(...){
		std::cout << "输入的参数中存在未定义的选项！\n";
		return 0;
	}
	//步骤四: 参数解析完毕，处理实际信息
	//count()检测该选项是否被输入
	if(vm.count("help") ){//若参数中有help选项
		//options_description对象支持流输出， 会自动打印所有的选项信息
		std::cout << opts << std::endl;
	}
	if(vm.count("filename") ){
		//variables_map(选项存储器)是std::map的派生类,可以像关联容器一样使用,
		//通过operator[]来取出其中的元素.但其内部的元素类型value_type是boost::any,
		//用来存储不确定类型的参数值,必须通过模板成员函数as<type>()做类型转换后,
		//才能获取其具体值.
		std::cout << "find " << vm["filename"].as<std::string>() << std::endl;
	}
	if(vm.empty() ){
		std::cout << "no options found \n";
	}
	return 0;
}
