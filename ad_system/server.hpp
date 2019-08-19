 #pragma once
#include "common/globalHelper.hpp"
#include "globalConfig.hpp"

using std::string;

namespace server {

class CServer: public http::CoreWorkInterface
{
public:
	CServer(globalConfig &config);
	~CServer();
	/* http_server的回调函数 */ 
	virtual bool Work(const string& in, string& out) override; 	
private:
	void Init();
    globalConfig &m_config;
};
}//namespace server 
