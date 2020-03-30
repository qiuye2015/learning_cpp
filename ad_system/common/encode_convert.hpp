#ifndef _ENCODE_CONVERT_H_
#define _ENCODE_CONVERT_H_
#include <iconv.h>
#include <string>
#include <cstring>

using namespace std;

// gb2312, gbk, utf-8, бн
bool _isUTF8_(const char* str);

bool _isGBK_(const char* str);

bool ConvertCore(
    const string& inCodeType, const string& outCodeType,
    const string& input, string& output);

// in:utf8, out:gbk
static bool _UTF8_to_GBK_(const string& in, string& out)
{
    return ConvertCore("utf-8", "GBK", in, out);
}

// in:gbk, out:utf8
static bool _GBK_to_UTF8_(const string& in, string& out)
{
    return ConvertCore("GBK", "utf-8", in, out);
}

// in:gb2312, out:utf8
static bool _GB2312_to_UTF8_(const string& in, string& out)
{
    return ConvertCore("gb2312", "utf-8", in, out);
}

// in:utf8, out:gb2312
static bool _UTF8_to_GB2312_(const string& in, string& out)
{
    return ConvertCore("utf-8", "gb2312", in, out);
}

#endif
