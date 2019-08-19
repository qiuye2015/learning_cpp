#include "globalHelper.hpp"
using std::string;

namespace globalHelper{


int GetIntInSettingFile(
						const string& frameName,
						const string& paraName,
						int defaultParaValue,
						const string& settingFileName){
	int rs = 0;
	string tmpRs;
	tmpRs = ReadStringFromIniFile(settingFileName, frameName, paraName);
	if (tmpRs.size() == 0) {
		rs = defaultParaValue;
	}
	else {
		try {
			rs = boost::lexical_cast<int>(tmpRs);
		}
		catch(...){
			rs = defaultParaValue;
		}
	}
	return rs;
}
string ReadStringFromIniFile(
							 const string& iniFileName,
							 const string& ref_desFrameName,
							 const string& ref_desItemName,
							 uint max_len){
	string rs;
	boost::regex reg("\\s*\\[(.*?)\\]\\s*.*");
	FILE *fp = fopen(iniFileName.c_str(), "r");
	if (NULL == fp) {
		ERROR("open file error: "+iniFileName + 
			  " [frame]: " + ref_desFrameName +
			  " [item]: " + ref_desItemName);
		rs.clear();
		return rs;
	}
	const int MY_MAX_LEN = max_len;
	char line[MY_MAX_LEN];
	boost::match_results<std::string::const_iterator> what;
	string desFrameName = ref_desFrameName; // 框架名称
	boost::to_lower(desFrameName);
	bool findFrame = false;
	string desItemName = ref_desItemName;   // 项目名称
	bool findItem = false;
	boost::to_lower(desItemName);
	string curFrame;
	string tmpLine;
	while (fgets(line, MY_MAX_LEN-1, fp)) {
		tmpLine = line;
		if (boost::regex_search(tmpLine,what,reg)) {
			if (what[1].matched) {
				curFrame.assign(what[1].first, what[1].second);
				boost::to_lower(curFrame);
				if (desFrameName == string(curFrame)) {
					findFrame = true;
					break;
				}
			}
		}
	}
	if (true == findFrame) {
		boost::regex regItem("\\s*([^\\s\\r\\=]*?)\\s*\\=(\\s*)([^\\r\\n]*?)([\\r\\n\\s]*)$"); // success
		string curLine;
		string part1;
		string part2;

		while (fgets(line, MY_MAX_LEN-1, fp)) {
			curLine = string(line);
			// 如果遇到下一个【frame】，则没有找到
			if (boost::regex_match(curLine, reg)) break;
			if (boost::regex_search(curLine,what,regItem)) {
				if (what[1].matched && what[3].matched) {
					part1.assign(what[1].first, what[1].second);
					part2.assign(what[3].first, what[3].second);
					boost::to_lower(part1);
					if (part1 == desItemName && part2.size()>0) {
						rs = part2;
						findItem = true;
						break;
					}
				}
			}
		}
	}
	if (findItem == false) {
		ERROR("open file error: "+iniFileName + 
			  " [frame]: " + ref_desFrameName +
			  " [item]: " + ref_desItemName);
	}
	fclose(fp);
	return rs;
}
// 返回当前时间 形式如：1980-11-17 10:00
string GetCurrentTime() {
	char curTime[1024];
	time_t now;
	time(&now);
	struct tm *tmnow;
	tmnow = localtime(&now);
	snprintf(
			 curTime,
			 1024,
			 "%d-%02d-%02d %02d:%02d:%02d",
			 tmnow->tm_year+1900,
			 tmnow->tm_mon+1,
			 tmnow->tm_mday,
			 tmnow->tm_hour,
			 tmnow->tm_min,
			 tmnow->tm_sec);

	return curTime;

}
/*boost::split
//按strSep切割字符串strIn，存入result
vector<string> splitString(const string& strIn,const string& strSep) {
	boost::regex sep(strSep);
	return std::vector<std::string>(
		   { boost::sregex_token_iterator(strIn.begin(),strIn.end(),sep,-1),
			 boost::sregex_token_iterator()});
}
*/
bool splitRequest(vector<string>& vecOut, const string& strIn) {
	vector<string> vecTemp;
	//vecTemp = splitString(strIn,"/");
	boost::split(vecTemp, strIn,boost::is_any_of("/"), boost::token_compress_on );
	for(unsigned i=0; i<vecTemp.size(); ++i){
		string strTemp = vecTemp[i];
		if (i==0 && strTemp=="") continue;
		if (_isGBK_(strTemp.c_str())) {
			_GBK_to_UTF8_(strTemp, strTemp);
		}
		strTemp = lowerString(strTemp);
		vecOut.push_back(strTemp);
	}
	return true;
}

string lowerString(const string& strIn) {
	string res(strIn);
	std::transform(res.begin(),res.end(),res.begin(),::tolower);
	return res;
}

}// namespace globalHelper
