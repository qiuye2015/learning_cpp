#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp> //boost::to_lower
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>//boost::is_any_of
#include <boost/thread.hpp>
#include <glog/logging.h>

#include "singleton.h"
#include "../server3/server.hpp"
#include "encode_convert.hpp"

using std::string;
using std::vector;

namespace globalHelper {

/********************LOG日志***********/
#define ERROR(log) std::cerr << "ERROR:" << __FILE__ << ":" << __LINE__ << "] " << log <<std::endl
#define INFO(log) std::cout << "INFO: " << log <<std::endl
#define ASSERT_TRUE(x) if(!x) {LOG(FATAL) << #x << " FAIL!";}
#define ASSERT_SUCC(x) if(AS_FAILED(x)) {LOG(FATAL) << #x << " FAIL!";}
#define ASSERT_NOT_NULL(x) if(!x) {LOG(FATAL) << #x << " is NULL!";} 
/**************读写ini格式文件***********/
int GetIntInSettingFile(const string& frameName,
						const string& paraName,
						int defaultParaValue,
						const string& settingFileName);
string ReadStringFromIniFile(const string&iniFileName, 
							 const string& ref_desFrameName, 
							 const string& ref_desItemName, 
							 uint max_len = 2048);

template<typename T>
bool getValue(const string &cfg, const string &section,
			  const string &key, T &value){
	string str_value = ReadStringFromIniFile(cfg, section, key);
	if(str_value == "") return false;
	try {
		value = boost::lexical_cast<T>(str_value);
	}
	catch(...) {
		return false;
	}
	return true;
}
/**************时间日期相关*************/
// 返回当前时间 形式如：1980-11-17 10:00
string GetCurrentTime();

/**************字符串操作相关*************/
//boost::split
//vector<string> splitString(string& strIn, const string& strSep);

bool splitRequest(vector<string>& vecOut, const string& strIn);
//字符串变小写
string lowerString(const string& strIn);
}// namespace globalHelper
