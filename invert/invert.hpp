/* g++ invert.cpp -std=c++11 -g -Ddebug
 * @Description: 倒排表
 * 1. indexMap 常驻内存
 * 2. invertVec 选择是否缓存(默认缓存)
 * 3. 创建倒排时可以选择是否持久化(默认持久化)
 * @Author: fjp
 * @Date: 2019-11-08 11:35:37
 */
#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;

//默认情况下，在Linux系统下，fopen和open操作的文件大小不能超过2G
#define _FILE_OFFSET_BITS 64

namespace invert
{
//--------------------------------------可更改start-------------------------------------
//三元组
struct PidKidInfo
{
    int nPid; // 索引ID=>key
    int nKid; // =>productID/ad_id
    int nWeight;
    PidKidInfo() {}
    PidKidInfo(int pid, int kid, int weight)
        : nPid(pid), nKid(kid), nWeight(weight)
    {
    }
};

struct InvertNode
{
    int nID; // info ID => productID
    int nWeight;
    InvertNode() {}
    InvertNode(int id, int weight)
        : nID(id), nWeight(weight)
    {
    }
    void copy(const PidKidInfo &other)
    {
        nID = other.nKid;
        nWeight = other.nWeight;
        //无 other.nPid
    }
};

// PidKidInfo 排序
template <typename _Info>
struct CmpPidKidInfoByPidWeight
{
    bool operator()(_Info l, _Info r)
    {
        if (l.nPid != r.nPid)
        {
            return l.nPid < r.nPid;
        }
        if (l.nWeight != r.nWeight)
        {
            return l.nWeight > r.nWeight;
        }
        return l.nKid < r.nKid;
    }
};
//--------------------------------------可更改end---------------------------------------

struct IndexNode
{
    int nID;     // 索引ID
    int nOffset; // mInvertVec 中偏移量
    int nCount;  // mInvertVec 中该索引ID的数量
};

/**
 * @description: 倒排表
 * @param {type} 
 * @return: 
 */
template <typename _InvertNode, typename _PidKidInfo>
class InvertTable
{
public:
    typedef _PidKidInfo mPidKidInfo; // 必须，要不invertCreator无法使用该类型;
private:
    // <IndexNode.nID,IndexID>(索引，索引对应的偏移量和数量)
    unordered_map<int, IndexNode> mIndexMap;
    // product 详情数组
    vector<_InvertNode> mInvertVec;

    // 缓存开关
    bool mUseCached;
    // 当不启用缓存时，打开invert文件的指针
    FILE *mFp;
    string mIndexFileName;
    string mInvertFileName;

public:
    InvertTable(bool useCache = true)
        : mUseCached(useCache), mIndexFileName(""), mInvertFileName(""), mFp(NULL)
    {
    }
    ~InvertTable()
    {
        clear();
    }

    /**
     * @description: 建立倒排表，设置输出目录
     * @param {vec}  待建立倒排的原始数据数组
     * @param {dir}  倒排持久化目录
     * @param {isPersistent}  是否持久化
     * @return: 
     */
    void create(vector<_PidKidInfo> &vec, const string &dir, bool isPersistent)
    {
        mIndexFileName = dir + "/Index.dict";
        mInvertFileName = dir + "/Invert.dict";

        // 清空缓存
        clear();
        // 在内存中建立倒排表
        createInvertTable(vec);
        // 持久化时，把倒排结构写入文件
        if (isPersistent)
        {
            writeIndex();
            writeInvert();
        }
        else if (!mUseCached)
        {
            /*如果没有持久化，选择不缓存的话，必须在创建倒排时写入文件*/
            writeInvert();
        }

        if (!mUseCached)
        {
            mInvertVec.clear();                         // 清空缓存vec
            mFp = fopen(mInvertFileName.c_str(), "rb"); // 打开倒排文件
        }
    }

    /**
     * @description: 读取文件，建立倒排表
     * @param {indexFile} 索引文件
     * @param {invertFile} 数据文件
     * @return: 
     */    
    bool load(const string &indexFile, const string &invertFile)
    {
        mIndexFileName = indexFile;
        mInvertFileName = invertFile;
        // 判断文件是否存在(0检查文件存在、2检查写权限、4读权限、6读写权限)
        if (access(mIndexFileName.c_str(), 0) != 0 || access(mInvertFileName.c_str(), 0) != 0)
            return false;

        // 读取索引文件到 mIndexMap
        readIndex();
        if (mUseCached)
        {
            return readInvert();
        }
        else
        {
            mFp = fopen(mInvertFileName.c_str(), "rb");
            return true;
        }
    }

    /**
     * @description: 读取某个索引对应的倒排
     * @param {index} 索引
     * @param {start} 返回该索引下倒排信息
     * @param {cnt} 返回该索引下倒排数量
     * @return: 
     */    
    bool getInvertInfo(int index, _InvertNode *&start, int &cnt)
    {
        // 索引不存在
        if (!indexIsExist(index))
        {
            cnt = 0;
            start = NULL;
            return false;
        }
        int offset = 0;
        // 返回倒排vector InvertNode的数量
        cnt = mIndexMap[index].nCount;
        offset = mIndexMap[index].nOffset;

        if (!mUseCached)
        {
            // 硬盘中读取数据
            return readInvert(offset, cnt, start);
        }
        // 缓存中取数据
        if (offset >= mInvertVec.size())
        {
            cnt = 0;
            start = NULL;
            return false;
        }
        start = &(mInvertVec[offset]);
        return true;
    }

    /**
     * @description: 读取某个索引对应的倒排到vector中
     * @param {index} 索引
     * @param {resultVec} 返回该索引下的倒排信息
     * @return: 
     */    
    bool getInvertInfo(int index, vector<_InvertNode> &resultVec)
    {
        // 清空 resultVec
        resultVec.clear();
        _InvertNode *_start = NULL;
        int cnt;
        if (!getInvertInfo(index, _start, cnt))
        {
            return false;
        }
        resultVec.insert(resultVec.begin(), _start, _start + cnt);
        return true;
    }

private:
    /**
     * @description: 清空缓存
     * @param {type} 
     * @return: 
     */    
    void clear()
    {
        mIndexMap.clear();
        mIndexMap.clear();
        if(mFp){
            fclose(mFp);
            mFp = NULL;
        }
    }

    /**
     * @description: 判断索引是否存在
     * @param {type} 
     * @return: 
     */
    bool indexIsExist(int index)
    {
        return (mIndexMap.find(index) != mIndexMap.end());
    }

    /**
     * @description: 在内存中建立倒排表
     * @param {infoVec} 待建立倒排的原始数据数组
     * @return: 
     */    
    void createInvertTable(vector<_PidKidInfo> &infoVec)
    {
        IndexNode indexNode;
        _InvertNode invertNode;

        int cnt = infoVec.size();
        if (0 == cnt)
        {
            return;
        }
        // 必须排序
        sort(infoVec.begin(), infoVec.end(), CmpPidKidInfoByPidWeight<_PidKidInfo>());

        mInvertVec.reserve(cnt);
        for (int i = 0; i < cnt; ++i)
        {
            if (0 == i)
            {
                // 重置IndexNode
                indexNode.nID = infoVec[i].nPid;
                indexNode.nOffset = i;
                indexNode.nCount = 0;
            }
            // 保存并新建索引节点
            if (indexNode.nID != infoVec[i].nPid)
            {
                mIndexMap[indexNode.nID] = indexNode;

                // 重置IndexNode
                indexNode.nID = infoVec[i].nPid;
                indexNode.nOffset = i;
                indexNode.nCount = 0;
            }
            // handle index
            indexNode.nCount += 1;

            // handle invert
            invertNode.copy(infoVec[i]);
            mInvertVec.push_back(invertNode);
        }
        // 保存结尾的索引节点
        mIndexMap[indexNode.nID] = indexNode;
    }

    /**
     * @description: 将 mIndexMap 写入文件 TODO:序列化
     * @param {type} 
     * @return: 
     */    
    void writeIndex()
    {
        FILE *fp = fopen(mIndexFileName.c_str(), "wb");
        for (auto iter = mIndexMap.begin(); iter != mIndexMap.end(); ++iter)
        {
            fwrite(&(iter->second), sizeof(IndexNode), 1, fp);
        }
        fclose(fp);
        fp = NULL;
    }
    /**
     * @description: 将 mInvertVec 写入文件
     * @param {type} 
     * @return: 
     */
    void writeInvert()
    {
        FILE *fp = fopen(mInvertFileName.c_str(), "wb");
        fwrite(&mInvertVec[0], sizeof(_InvertNode), mInvertVec.size(), fp);
        fclose(fp);
        fp = NULL;
    }
    /**
     * @description: 硬盘中读取数据到 mIndexMap
     * @param {type} 
     * @return: 
     */
    void readIndex()
    {
        int fd = open(mIndexFileName.c_str(), O_RDONLY);
        struct stat st;
        fstat(fd, &st); /*获取文件大小*/
        void *start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (MAP_FAILED == start)
        { /*判断内存映射是否成功*/
            return;
        }
       
        int cnt = st.st_size / sizeof(IndexNode);
        IndexNode *p = (IndexNode *)start;
        for (int i = 0; i < cnt; ++i)
        {
            if (p->nCount != 0)
            {
                mIndexMap[p->nID] = *p;
            }
            ++p;
        }
        munmap(start, st.st_size); //解除映射
        p = NULL;
        close(fd);
    }
    /**
     * @description: 硬盘中读取数据到 mInvertVec
     * @param {type} 
     * @return: 
     */
    bool readInvert()
    {
        #if 0
        FILE *fp = fopen(mInvertFileName.c_str(), "rb");
        fseek(fp, 0, SEEK_END); // 文件结尾
        int cnt = ftell(fp) / sizeof(_InvertNode);

        fseek(fp, 0, SEEK_SET); // 文件开头
        mInvertVec.reserve(cnt);
        int read_cnt = fread(&mInvertVec[0], sizeof(_InvertNode), cnt, fp);
        if (read_cnt == -1 || read_cnt != cnt){
            mInvertVec.clear();
        }
        fclose(fp);
        fp = NULL;
        return true;
        #else
        int fd = open(mInvertFileName.c_str(), O_RDONLY);
        struct stat st;
        fstat(fd, &st); /*获取文件大小*/
        void *start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (MAP_FAILED == start)
        { /*判断内存映射是否成功*/
            return false;
        }
        
        int cnt = st.st_size / sizeof(_InvertNode);
        mInvertVec.reserve(cnt);
        _InvertNode *p = (_InvertNode *)start;
        mInvertVec.insert(mInvertVec.begin(),p,p+cnt);
        munmap(start, st.st_size); //解除映射
        p = NULL;
        close(fd);
        return true;
        #endif
    }

    /**
     * @description:  硬盘中读取特定数据
     * @param {type} : TODO:mmap
     * @return: 
     */
    bool readInvert(int offset, int cnt, _InvertNode *&start)
    {
        if(NULL != start){
            delete[] start;//防止下面的new的内存泄漏
            start = NULL;
        }
        long long addr = offset * sizeof(_InvertNode);
        if (fseek(mFp, addr, 0) == -1)
        {
            start = NULL;
            return false;
        }
        start = new _InvertNode[cnt];
        int read_cnt = fread(start, sizeof(_InvertNode), cnt, mFp);

        if (read_cnt == -1 || read_cnt != cnt)
        {
            delete[] start;
            start = NULL;
            return false;
        }
        return true;
    }
};
} //end namespace invert

// 批量注释测试代码
#if debug_1
using namespace invert;
int main()
{
    cout << "test invert table start..." << endl;
    srand((unsigned int)time(NULL)); //设置随机数种子
    vector<PidKidInfo> vec;
    PidKidInfo info;
    int num = 0;
    for (int i = 0; i < 10; ++i)
    {
        info.nPid = rand() % 10; // key
        info.nKid = num++;
        info.nWeight = rand() % 100;
        vec.push_back(info);
    }

    cout << "--------原始数据---------------" << endl;
    // for_each(vec.begin(), vec.end(), output);
    for_each(vec.begin(), vec.end(), [](PidKidInfo &info){
        cout << info.nPid << " " << info.nKid << " " << info.nWeight << endl;
    });
    cout << "------------------------------" << endl;

    InvertTable<InvertNode, PidKidInfo> IVT(true);
    IVT.create(vec,".",true);
    // IVT.load("Index.dict", "Invert.dict");

    cout << "----------倒排索引map----------" << endl;
    for (auto iter = IVT.mIndexMap.begin(); iter != IVT.mIndexMap.end(); ++iter)
    {
        cout << iter->first << " | "
             << " " << iter->second.nOffset << " " << iter->second.nCount << endl;
    }
    cout << "-----------------------------" << endl;
    int c = 0;
    cout << "----------倒排索引vec----------" << endl;

    for (auto iter = IVT.mInvertVec.begin(); iter != IVT.mInvertVec.end(); ++iter)
    {
        cout << c++ << " | " << iter->nID << endl;
    }
    cout << "-------------------------------" << endl;
    InvertNode *start = NULL;
    int cnt = 0;
    cout << "----------倒排索引test----------" << endl;
    for (int j = 0; j < 10; j++)
    {
        if (IVT.getInvertInfo(j, start, cnt))
        {
            for (int k = 0; k < cnt; ++k, ++start)
            {
                cout << j << " | " << start->nID << " " << start->nWeight << endl;
            }
            cout << "================" << endl;
        }
    }
    cout << "----------倒排索引test2----------" << endl;
    vector<InvertNode> out;
    for (int j = 0; j < 10; j++)
    {
        if (IVT.getInvertInfo(j, out))
        {
            for (int k = 0; k < out.size(); ++k)
            {
                cout << j << " | " << out[k].nID << " " << out[k].nWeight << endl;
            }
            cout << "================" << endl;
        }
    }
    cout << "test invert table end" << endl;
}
#endif