#include <iostream>
#include <glog/logging.h>  
//g++ -o test_glog  -lglog test_glog.cpp  
int main(int argc,char* argv[]) {  
    // If glog is used to parse the command line   
    // google::ParseCommandLineFlags(&argc, &argv, true);  
  
    // Initialize Google's logging library.  
    google::InitGoogleLogging(argv[0]);  
  
    FLAGS_log_dir = "./log";  
  
    LOG(INFO) << "hello world";  
  
    return 0;  
}  
