#ifndef HTTP_SERVER3_CORE_WORK_INTERFACE_HPP
#define HTTP_SERVER3_CORE_WORK_INTERFACE_HPP
#include <string>

namespace http {

class CoreWorkInterface
{
public:
	CoreWorkInterface() {}
	virtual ~CoreWorkInterface() {}
	//提供http业务处理服务,具体由子类实现
	virtual bool Work(const std::string& request, std::string& rep_string) = 0; 
};
}//namespace http

#endif // HTTP_SERVER3_CORE_WORK_INTERFACE_HPP
