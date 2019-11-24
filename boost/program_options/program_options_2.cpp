//计算橘子和苹果的总数量，可以指定多个生产地 //
//编译选项加上 -lboost_program_options //
///////////////////////////////////////////
#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
namespace bpo = boost::program_options;
int main(int argc, char const *argv[])
{
	//外部变量，用于保存获取的参数值
	int apple_num = 0, orange_num = 0;
	std::vector<std::string> addr;
	bpo::options_description opt("all options");
	//指定该参数的默认值
	opt.add_options()
		("apple,a", bpo::value<int>(&apple_num)->default_value(10), "苹果的数量")
		("orange,o", bpo::value<int>(&orange_num)->default_value(20), "橘子的数量")
		("address", bpo::value<std::vector<std::string> >()->multitoken(), "生产地")
		("help", "计算苹果和橘子的总数量");
	// "apple,a" : 指定选项的全写形式为 --apple, 简写形式为 -a
	// value<type>(ptr) : ptr为该选项对应的外部变量的地址, 当该选项被解析后,
	// 可通过下面的notify()函数将选项的值赋给该外部变量,该变量的值会自动更新
	// defaut_value(num) : num为该选项的默认值, 若命令行中未输入该选项, 则该选项的值取为num
	// 该参数的实际类型为vector,所以命令行中输入的值可以是多个,
	// multitoken()的作用就是告诉编译器,该选项可接受多个值
	bpo::variables_map vm;
	try{
		bpo::store(parse_command_line(argc, argv, opt), vm);
	}
	catch(...){
		std::cout << "输入的参数中存在未定义的选项！\n";
		return 0;
	}
	//参数解析完成后，通知variables_map去更新所有的外部变量
	//这句话执行后, 会使得apple_num和orange_num的值自动更新为选项指定的值
	bpo::notify(vm);
	if(vm.count("help") ){
		std::cout << opt << std::endl;
		return 0;
	}
	if(vm.count("address") ){
		std::cout << "生产地为：";
		//遍历选项值
		for(auto& str : vm["address"].as<std::vector<std::string> >() )
			std::cout << str << " ";
			std::cout << std::endl;
		}
	std::cout << "苹果的数量:" << apple_num << std::endl;
	std::cout << "橘子的数量:" << orange_num << std::endl;
	std::cout << "总数量数量:" << orange_num + apple_num << std::endl;
	return 0;
}
