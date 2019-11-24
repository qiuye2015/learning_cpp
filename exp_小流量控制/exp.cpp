#include"exp.h"

bool isBalnk(char c)
{
    return ((c == ' ') || (c == '\t') || (c == '\n') ||
            (c == '\r') || (c == '\0') || (c == '\v'));
}

string trim(string& str)
{
    size_t len = str.size();
    if(len == 0)
        return string("");
    size_t start = 0;
    size_t end = len-1;
    while((start<end) && isBalnk(str[start]))
        start++;
    while((end>start) && isBalnk(str[end]))
        end--;
    if(start == end)
        return string("");
    return str.substr(start,end-start+1);
}

void Exp::load(string filename)
{
    ifstream ifs(filename.c_str());
    if(!ifs)
    {
        cout<<"file: "<<filename<<" not exist!"<<endl;
        return;
    }

    string tmp = "";
    string posid = "";
    map<string,expNode> flag2value;
    int i = 0;
    while(getline(ifs,tmp))
    {
        i++;
        transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
        size_t len = tmp.size();
        if((len == 0) || (tmp[0] == '#'))
            continue;
        if((tmp[0] == '[') && (tmp[len-1] == ']'))
        {
            if(posid == "default")
            {
                m_default = flag2value;
                flag2value.clear();
                posid = "";
                continue;
            }
            else if(posid != "")
            {
                m_conf.insert(make_pair(posid,flag2value));
                flag2value.clear();
            }
            posid = tmp.substr(1,len-2);
            posid = trim(posid);
            continue;
        }
        size_t pos = tmp.find("=");
        if(pos == string::npos)
        {
            cout<<"exp line "<<i<<" is not right!"<<endl;
            continue;
        }
        string flag = tmp.substr(0,pos);
        flag = trim(flag);
        string value = tmp.substr(pos+1);
        value = trim(value);
        expNode expnode;
        if(flag.find("enable") != string::npos)
        {
            if(value == "true")
                expnode.flagvalue.bl = true;
            else if(value == "false")
                expnode.flagvalue.bl = false;
            else
            {
                cout<<"line "<<i<<" is wrong!"<<endl;
                continue;
            }
            expnode.flagtype = BOOL;
        }
        else if(flag.find("int") != string::npos)
        {
            expnode.flagvalue.it = atoi(value.c_str());
            expnode.flagtype = INT;
        }
        else if(flag.find("float") != string::npos)
        {
            expnode.flagvalue.ft = (float)atof(value.c_str());
            expnode.flagtype = FLOAT;
        }
        else if(flag.find("string") != string::npos)
        {
            expnode.flagvalue.p = new char[value.size()];
            expnode.flagvalue.p = value.c_str();
            expnode.flagtype = STRING;
        }
        else
        {
            cout<<"exp is not right!"<<endl;
            continue;
        }
        flag2value.insert(make_pair(flag,expnode));
    }
    if(posid == "default")
    {
        m_default = flag2value;
        flag2value.clear();
    }
    else if(posid != "")
    {
        m_conf.insert(make_pair(posid,flag2value));
        flag2value.clear();
    }
}

expNode& Exp::getDefault(string flag)
{
    if(m_default.find(flag) == m_default.end())
    {
        cout<<"get "<<flag<<" not exist!"<<endl;
        exit(-1);
    }
    return m_default[flag];
}

expNode& Exp::getExpNode(string posid,string flag)
{
    if(m_conf.find(posid) == m_conf.end())
    {
        cout<<"posid: "<<posid <<" is not find!"<<endl;
        return getDefault(flag);
    }
    else
    {
        map<string,expNode> flag2value = m_conf[posid];
        if(flag2value.find(flag) == flag2value.end())
        {
            cout<<"posid: "<<posid<<" do not have flag: "<<flag<<endl;
            return getDefault(flag);
        }
        return flag2value[flag];
    }
}

FlagType Exp::getType(string flag)
{    
    FlagType flagtype = NONE;
    if(flag.find("enable")!=string::npos)
        flagtype = BOOL;
    else if(flag.find("int")!=string::npos)
        flagtype = INT;
    else if(flag.find("float")!=string::npos)
        flagtype = FLOAT;
    else if(flag.find("string")!=string::npos)
        flagtype = STRING;
    return flagtype;
}

expNode Exp::judgeExpNode(string posid, string flag, FlagType ft)
{
    transform(posid.begin(), posid.end(), posid.begin(), ::tolower);
    transform(flag.begin(), flag.end(), flag.begin(), ::tolower);
    expNode& expnode = getExpNode(posid,flag);

    FlagType flagtype = getType(flag);

    if((expnode.flagtype != flagtype) || (flagtype != ft))
    {
        cout<<"flag: "<<flag<<" type is wrong"<<endl;
        exit(-2);
    }
    return expnode;
}

bool Exp::getBoolValue(string posid, string flag)
{
    return judgeExpNode(posid,flag,BOOL).flagvalue.bl;
}
int Exp::getIntValue(string posid, string flag)
{
    return judgeExpNode(posid,flag,INT).flagvalue.it;
}
float Exp::getFloatValue(string posid, string flag)
{
    return judgeExpNode(posid,flag,FLOAT).flagvalue.ft;
}
const char* Exp::getStringValue(string posid, string flag)
{
    return judgeExpNode(posid,flag,STRING).flagvalue.p;
}
