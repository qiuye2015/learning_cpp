/*
 * @Description: 倒排创建器
 * 1. 辅助创建倒排表
 * 2. 通过addTripe逐条插入数据到vector中，再传递给倒排表
 * 3. 构造函数决定是否持久化。(默认持久化)
 * @Author: fjp
 * @Date: 2019-11-11 16:19:37
 */
// #pragma once
#include <iostream>
#include <string>
#include <set>
#include <utility>//pair
// #include <filesystem>

#include "invert.hpp"

using namespace std;

namespace invert{

template<typename _InvertTable>
class InvertCreator{
private:
    typedef typename _InvertTable::mPidKidInfo _mPidKidInfo;

    // 是否持久化
    bool mIsPersistent;
    // 持久化目录
    string mPath;

    // 待建立倒排数组
    vector<_mPidKidInfo> mTripleVec;
    // 倒排表
    _InvertTable *pInvertTable;
    // 根据倒排中索引和productID去重
    set<pair<int, int>> mFilterSet;
    
public:
    InvertCreator(_InvertTable &invertTable, string dirStr="./", bool isPersistent=true)
    : pInvertTable(&invertTable), mIsPersistent(isPersistent), mPath(dirStr)
    {
        // 创建目录TODO:C++17 正式加入
        // using namespace boost;
        // filesystem::path dir(mPath);
        // if(!filesystem::exists(dir)){
        //     filesystem::create_directories(dir);
        // }

    }
    ~InvertCreator(){
        mTripleVec.clear();
        mFilterSet.clear();
    }
    /**
     * @description: 添加 <indexID,invertID,weight> 三元组
     * @param {type} 
     * @return: 
     */
    void addTriple(int indexID, int invertID, int weight){
        pair<int, int> key = make_pair(indexID, invertID);
        if(mFilterSet.find(key) == mFilterSet.end()){
            mFilterSet.insert(key);
            mTripleVec.push_back(_mPidKidInfo(indexID, invertID, weight));
        }
    }

    /**
     * @description: 当所有元素调用addTriple 添加到数组mTripleVec后, 开始构建倒排表
     * @param {type} 
     * @return: 
     */
    void create(){
        pInvertTable->create(mTripleVec, mPath, mIsPersistent);
    }

};
};// end namespace invert

#if debug_2
int main(){
    cout<<"test InvertCreator start..."<<endl;
    using namespace invert;
    // 创建倒排表&使用缓存
    typedef InvertTable<InvertNode, PidKidInfo> tInvertTable;
    tInvertTable IVT(true);
    // 使用倒排表创建倒排创建器&持久化
    InvertCreator<tInvertTable> ICT(IVT,"./", true);

    srand((unsigned int)time(NULL)); //设置随机数种子
    int num = 0;
    for(int i = 0;i<10; ++i){
        int Pid = rand() % 10; // key
        int Kid = num++;
        int Weight = rand() % 100;
        ICT.addTriple(Pid, Kid, Weight);
        cout<<i<<" | "<<Pid<<" "<<Kid<<" " <<Weight<<endl;
    }
    ICT.create();

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
    cout<<"test InvertCreator end!"<<endl;
}

#endif