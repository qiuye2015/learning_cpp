/*
 * �������ű�ĸ����ࣺ
 * 1. �����������ݣ���ָ���ֶ�����󣬽�vector�������ű�
 * 2. createʱ��ѡ���Ƿ�־û�����Ĭ�ϳ־û���
 */

#ifndef INVERT_TABLE_CREATOR_H_
#define INVERT_TABLE_CREATOR_H_

#include <boost/filesystem.hpp>
#include "invert_table.h"

namespace fjp
{

// ����ʽ
enum SortMethod
{
	byIndex,	// ֻ��index���ȶ�������
	byID,		// ��id����
	byWeight,	// ��weight����
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
	/* ֻ�ṩһ����ʼ������ */
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
		//����������·���ļ���δ�����������Ƚ���
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

	// ����<index_id, invert_id, weight>��Ԫ��
	inline void addTriple(int index_id, int invert_id, int weight)
	{
		pair<int, int> k = make_pair(index_id, invert_id);
		if(dup_filter.find(k) == dup_filter.end())
		{
			m_tripleVec.push_back(_ST_PidKidInfo(index_id, invert_id, weight));
			dup_filter.insert(k);
		}
	}

	// ����<index_id, invert_id, weight, weight2>��Ԫ��
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

	// ��ȫ��Ԫ�������ϣ��������ű�
	void create()
	{
		switch(m_sortMethod)
		{
			case byIndex:
				// �ȶ�������
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
