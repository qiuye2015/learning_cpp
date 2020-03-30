#ifndef Z_INVERT_TABLE_H
#define Z_INVERT_TABLE_H

#include <vector>
#include <map>
#include <set>
#include "invert_table.h"

using namespace std;

namespace adrank{

template <typename _InvertNode, typename _ST_PidKidInfo>
class ZInvertTable
{
public:
	typedef _InvertNode InvertNodeType;
	typedef _ST_PidKidInfo ST_PidKidInfoType;

public:
	void insert(_ST_PidKidInfo info)
	{
		pair<int, int> k = make_pair(info.m_nPid, info.m_nKid);
		if (dup_filter.count(k))
			return;

		_InvertNode node;
		node.copy(info);
		id2Nodes[info.m_nPid].push_back(node);
		dup_filter.insert(k);
	}

	void create(vector<_ST_PidKidInfo> &vec, const char *dir, bool is_persist)
	{
		id2Nodes.clear();
		dup_filter.clear();

		createInvertTable(vec);
	}

	void createInvertTable(vector<_ST_PidKidInfo> &vec)
	{
		for (int i=0; i < vec.size(); i++)
		{
			insert(vec[i]);
		}
	}

	bool getPostList(int id, vector<_InvertNode> &nodes, _InvertNode* &start, int &cnt)
	{
		typename map<int, vector<_InvertNode> >::iterator it = id2Nodes.find(id);

		if (it == id2Nodes.end())
		{
			cnt = 0;
			start = NULL;
			return false;
		}

		start = &it->second[0];
		cnt = it->second.size();
		return true;
	}

public:
	map<int, vector<_InvertNode> > id2Nodes;
	set<pair<int, int> > dup_filter;
};

}

#endif
