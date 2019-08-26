#include "serverframe.hpp"
#include "common/globalHelper.hpp"

using std::cout;
using std::endl;
namespace server {

ServerFrame::ServerFrame(const string &setting, const string &exp)
 : m_config(setting){
    Init();
}

ServerFrame::~ServerFrame() {}

void ServerFrame::Init() {
    server = new CServerWork(m_config);
    ASSERT_NOT_NULL(server);
}

void ServerFrame::Run() {
    server_thread = new boost::thread(boost::bind(
                    &ServerFrame::StartService, this));
    server_thread->join();
    /* 走不到 */
    delete server_thread; 
    server_thread = NULL;
}	

void ServerFrame::StartService() {
    int n = 0;
    string host = "0.0.0.0";
    string root_path = "./wwwroot";
    
    //while(true){
        try {
            s = new http::server3::server(host, 
                m_config.service_port, 
                root_path, 
                m_config.service_thread_num, 
                "txt");
        }
        catch(...) {
            ERROR("service port [" + m_config.service_port + "] already used");
        }
        s->setCoreWorker(server);
        INFO("server work, port [" + m_config.service_port + "]");
        s->run();
        delete s;
        s = NULL;
        //n++;
        //cout << "INFO: server restart [" << n << "] times" << endl;
    //}
}

}// namespace server
