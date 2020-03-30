#include "serverWork.hpp"
#include <unistd.h>

namespace server {

CServerWork::CServerWork(globalConfig &config)
 : m_config(config){
    Init();
}
CServerWork::~CServerWork() {}

void CServerWork::Init() {
    INFO("CServer::Init");
}
bool CServerWork::Work(const string& in, string& out) {
    if (m_config.enable_print_request) {
        LOG(INFO)<<"CServer::Work start>> "<< in;
    }
    vector<string> vecIn;
    globalHelper::splitRequest(vecIn,in);
    out="process result: " + in;

    for(int i=0;i<1000000000;++i){
        ;
    }
    if (m_config.enable_print_request) {
        LOG(INFO)<<"CServer::Work end>>>> "<< out;
    }
    return true;
}


}//namespace server 
