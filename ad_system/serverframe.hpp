 #pragma once
#include "common/globalHelper.hpp"
#include "globalConfig.hpp"
#include "server.hpp"

using std::string;

namespace server {

class ServerFrame {
public:
	ServerFrame(const string &setting, const string &exp);
	~ServerFrame();
	void Run();
private:
	void Init();
	void StartService();
private:
	CServer *server;
	boost::thread *server_thread;
	http::server3::server *s;
    globalConfig m_config;
};
}// namespace server
