#include <iostream>
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;
unsigned splitString(vector<string>& result, const string& strIn,
					 const string& strSep) {
	boost::regex sep(strSep);
	result =  std::vector<std::string>(
									   {boost::sregex_token_iterator(strIn.begin(),strIn.end(),sep,-1),                                              
									   boost::sregex_token_iterator()});
}  
int main(int argc,char* argv[])
{
	string s;
	s="/getAds/cpc/6/0/10/7240/184537119/0/0/0/107169205/1/-/0/23934469/0/0/-";
	vector<string> out;
	//splitString(out,s,"/");
	boost::split(out,s, boost::is_any_of("/"),boost::token_compress_on);
	cout<<out.size()<<endl;
	for(int i=0;i<out.size();++i){
		cout<<out[i]<<" | ";
		}
	return 0;
}
