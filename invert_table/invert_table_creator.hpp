/*
 * 创建倒排表的辅助类：
 * 1. 逐条插入数据，按指定字段排序后，将vector传给倒排表。
 * 2. create时可选择是否持久化。（默认持久化）
 */

#ifndef INVERT_TABLE_CREATOR_H_
#define INVERT_TABLE_CREATOR_H_

#include <boost/filesystem.hpp>
#include "invert_table.h"

namespace fjp
{

// 排序方式
enum SortMethod
{
	byIndex,	// 只排index，稳定的排序
	byID,		// 按id排序
	byWeight,	// 按weight排序
};

template<typename _InvertTable>
class InvertTableCreator
{
public:
	typedef typename _InvertTable::ST_PidKidInfoType _ST_PidKidInfo;
public:
	std::string m_dir;
	bool m_persistent;
	SortMethod m_sortMethod;

	_InvertTable *m_invertTable;
	std::vector<_ST_PidKidInfo> m_tripleVec;
	set<pair<int, int> > dup_filter;


public:
	/* 只提供一个初始化方法 */
	InvertTableCreator(_InvertTable &invert_table, string dir,
		SortMethod sort_method, bool is_persistent = true)
	: m_invertTable(&invert_table), m_dir(dir), m_sortMethod(sort_method),
	  m_persistent(is_persistent)
	{
		init();
	}
	
	~InvertTableCreator()
	{
		clear();
	}

	void init()
	{
		namespace bf = boost::filesystem;
		bf::path path(m_dir);
		//如果索引输出路径文件夹未被建立则首先建立
		if (!bf::exists(path))
		{
			bf::create_directories(path);
		}
	}

	void clear()
	{
		m_tripleVec.clear();
		dup_filter.clear();
	}

	// 加入<index_id, invert_id, weight>三元组
	inline void addTriple(int index_id, int invert_id, int weight)
	{
		pair<int, int> k = make_pair(index_id, invert_id);
		if(dup_filter.find(k) == dup_filter.end())
		{
			m_tripleVec.push_back(_ST_PidKidInfo(index_id, invert_id, weight));
			dup_filter.insert(k);
		}
	}

	// 加入<index_id, invert_id, weight, weight2>四元组
	inline void addTriple(int index_id, int invert_id, int weight, int weight2)
	{
		pair<int, int> k = make_pair(index_id, invert_id);
		if(dup_filter.find(k) == dup_filter.end())
		{
			m_tripleVec.push_back(_ST_PidKidInfo(index_id, invert_id,
				weight, weight2));
			dup_filter.insert(k);
		}
	}

	// 当全部元组加入完毕，建立倒排表
	void create()
	{
		switch(m_sortMethod)
		{
			case byIndex:
				// 稳定的排序
				std::stable_sort(m_tripleVec.begin(), m_tripleVec.end(), CmpSTNodeByIndex());
				break;

			case byID:
				std::sort(m_tripleVec.begin(), m_tripleVec.end(), CmpSTNodeByID());
				break;

			case byWeight:
				std::sort(m_tripleVec.begin(), m_tripleVec.end(), CmpSTNodeByWeight());
				break;

			default:
				std::sort(m_tripleVec.begin(), m_tripleVec.end(), CmpSTNodeByIndex());
				break;
		}

		m_invertTable->create(m_tripleVec, m_dir.c_str(), m_persistent);
	}

};

typedef InvertTableCreator<InvertTable2> InvertTableCreator2;

} //----end of namespace fjp

#endif  //----end of INVERT_TABLE_CREATOR_H_
