#include <gperftools/profiler.h>
#include "common/globalHelper.hpp"
#include "glog/logging.h"
#include "serverframe.hpp"

void gprofStartAndStop(int signum) {
    static int isStarted = 0;
    if (signum != SIGUSR1) return;

    //通过isStarted标记未来控制第一次收到信号量开启性能分析，
    //第二次收到关闭性能分析。
    if (!isStarted){
        isStarted = 1;
        ProfilerStart("test.prof");
        INFO("ProfilerStart success");
    }else{
        ProfilerStop();
        INFO("ProfilerStop success");
    }
}
int main(int argc, char*argv[])
{
    //通过信号控制性能分析
    signal(SIGUSR1, gprofStartAndStop);

    using std::string;
    string setting = "config/server.ini";
    string exp = "config/exp.conf";
    if(argc == 2)
    {
        setting = argv[1];
    }

    #define GET_GLOG(section,defauleValue) \
        globalHelper::GetIntInSettingFile("GLOG",section,defauleValue,setting)
    /* 初始化 GoogleLogging */
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = "./log";
    FLAGS_logbufsecs = 0;//缓冲日志输出，默认为30秒，此处改为立即输出
    FLAGS_max_log_size = GET_GLOG("glog_max_size", 2048);
    FLAGS_minloglevel = GET_GLOG("glog_min_log_level", 0);//默认0，INFO

    //设置日志消息是否转到标准输出而不是日志文件
    FLAGS_logtostderr =GET_GLOG("glog_logtostderr",0)==1;//默认只输出到日志文件
    //设置记录到标准输出的颜色消息（如果终端支持）
    FLAGS_colorlogtostderr =GET_GLOG("glog_logtostderr",0)==1;
    //输出到标准输出日志级别
    if(FLAGS_logtostderr)
        FLAGS_stderrthreshold = GET_GLOG("glog_min_log_level",0); //默认0，INFO

    #undef GET_GLOG

    server::ServerFrame serverFrame(setting, exp);
    serverFrame.Run();

    return 0;
}

