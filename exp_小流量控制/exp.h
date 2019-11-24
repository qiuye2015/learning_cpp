#ifndef _EXP_H_
#define _EXP_H_

#include<map>
#include<string>
#include<fstream>
#include<stdlib.h>
#include<iostream>
#include <algorithm>

using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::map;
using std::ifstream;

typedef union{
        bool bl;
        int it;
        float ft;
        const char *p;
} FlagValue;

enum FlagType{
    NONE = -1,
    BOOL = 0,
    INT = 1,
    FLOAT =2,
    STRING = 3,
};

struct expNode{
    FlagType flagtype;
    FlagValue flagvalue;
};

bool isBalnk(char c);

string trim(string& str);

class Exp{
    public:
        Exp(string filename)
        {
            load(filename);
        }
        bool getBoolValue(string posid,string pflag);
        int getIntValue(string posid,string pflag);
        float getFloatValue(string posid,string pflag);
        const char* getStringValue(string posid,string pflag);
    private:
        void load(string filename);
        FlagType getType(string flag);
        expNode judgeExpNode(string posid, string flag, FlagType ft);
        expNode& getDefault(string flag);
        expNode& getExpNode(string posid,string flag);
        map<string,map<string,expNode> > m_conf;
        map<string,expNode> m_default;
};

#endif
