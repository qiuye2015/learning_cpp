/*
 * ���ű�
 * 1. index�᳣ܻפ�ڴ棬û�����ݵ�index�ڵ㱻ɾ���ˣ�������Ҫע�⡣
 * 2. invert��ѡ���Ƿ񻺴档��Ĭ�ϻ��棩
 * 3. createʱ��ѡ���Ƿ�־û�������������ʱ��invert�ܻ�־û���
 */
#ifndef INVERT_TABLE_H_
#define INVERT_TABLE_H_

#include <stdio.h>
#include <iostream>
#include <vector>
#include <set>
#include <functional>
#include <algorithm>
#include <cmath>
#include <tr1/unordered_map>
#include <sys/mman.h>
#include <sys/io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "types.h"

#include "z_invert_table.h"

using namespace std;

//Ĭ������£���Linuxϵͳ�£�fopen��open�������ļ���С���ܳ���2G
#define _FILE_OFFSET_BITS 64
typedef long long __int64

namespace fjp {

// ��Ԫ��
struct ST_PidKidInfo2
{
	int m_nPid;
	int m_nKid;
	int m_nWeight;
	int m_nWeight2;

	ST_PidKidInfo2()
	{}

	ST_PidKidInfo2(int pid, int kid, int weight, int weight2)
	: m_nPid(pid), m_nKid(kid), m_nWeight(weight), m_nWeight2(weight2)
	{}
};

// ��Ԫ���InvertNode��ȥ��indexID��Ϊ��Ԫ��
struct InvertNode2
{
	int m_nID;
	int m_nWeight;
	int m_nWeight2;

	InvertNode2()
	{}

	InvertNode2(int id, int weight, int weight2)
	: m_nID(id), m_nWeight(weight), m_nWeight2(weight2)
	{}

	inline void copy(const ST_PidKidInfo2 &other)
	{
		m_nID = other.m_nKid;
		m_nWeight = other.m_nWeight;
		m_nWeight2 = other.m_nWeight2;
	}
};

// IndexNode
struct IndexNode
{
	int m_nID;
	int m_nOffset;
	int m_nCount;
	int m_nTotalWeight;
};

/*
 * STNode�����򷽷�
 * ��invert_table_creater����
 */
struct CmpSTNodeByIndex
{
	template<typename _STNode>
	bool operator() (const _STNode &i, const _STNode &j)
	{
		return i.m_nPid < j.m_nPid;
	}
};

struct CmpSTNodeByID
{
	template<typename _STNode>
	bool operator() (const _STNode &i, const _STNode &j)
	{
		if(i.m_nPid != j.m_nPid)
		{
			return i.m_nPid < j.m_nPid;
		}

		return i.m_nKid < j.m_nKid;
	}
};

struct CmpSTNodeByWeight
{
	template<typename _STNode>
	bool operator() (const _STNode &i, const _STNode &j)
	{
		if(i.m_nPid != j.m_nPid)
		{
			return i.m_nPid < j.m_nPid;
		}

		if(i.m_nWeight != j.m_nWeight)
		{
			return i.m_nWeight > j.m_nWeight;
		}

		return i.m_nKid < j.m_nKid;
	}
};

/*
 * InvertNode�����򷽷�
 * ע�⣺����create��load�����в����InvertNode��������
 *       �����vec�����ļ����Ѿ��������

struct CmpIvtNodeByID
{
    template<typename _IvtNode>
    bool operator() (const _IvtNode &i, const _IvtNode &j) 
    {    
        return (i.m_nID < j.m_nID);
    }    
};

struct CmpIvtNodeByWeight
{
    template<typename _IvtNode>
    bool operator() (const _IvtNode &i, const _IvtNode &j) 
    {    
        return (i.m_nWeight > j.m_nWeight);
    }    
};
*/

/*
 * ���ű�
 */
template <typename _InvertNode, typename _ST_PidKidInfo>
class InvertTable
{
public:
	// ���濪��
	bool is_cached;
	// �������û���ʱ����invert�ļ���ָ��
	FILE *fp;

	string index_file_name;
	string invert_file_name;

	// index, invert
	tr1::unordered_map<int, IndexNode> index_map;
	vector<_InvertNode> invert_vec;

public:
	InvertTable(bool use_cache = true)
	: index_file_name(""), invert_file_name(""), is_cached(use_cache),
	  fp(NULL)
	{}

	~InvertTable()
	{
		clear();
	}

	inline bool has_key(int index_id)
	{
		return (index_map.find(index_id) != index_map.end());
	}

	void clear()
	{
		if(fp)
		{
			fclose(fp);
			fp = NULL;
		}

		index_map.clear();
		invert_vec.clear();
	}

	// �������ű��������Ŀ¼
	void create(vector<_ST_PidKidInfo> &vec, const char *dir, bool is_persistent)
	{
		// ���õ����ļ���
		index_file_name = string(dir) + "/index.idx";
		invert_file_name = string(dir) + "/invert.idx";

		// ��ջ���
		clear();

		// �ڴ��н������ű�
		createInvertTable(vec);

		// �־û����ѵ��Žṹд���ļ�
		if(is_persistent)
		{
			writeIndex();
			writeInvert();
		}
		else if(!is_cached)
		{
			writeInvert();
		}

		// ��������棬�򿪵����ļ�����յ���vec
		if(!is_cached)
		{
			fp = fopen(invert_file_name.c_str(), "rb");
			invert_vec.clear();
		}
	}

	/*
	 * ��vector�ж�ȡ���ݣ��������ű�
	 * ע�⣺���vector����Ҫ��<index_id>�Ź���
	 */
	void createInvertTable(const vector<_ST_PidKidInfo> &tmp_info)
	{
		IndexNode index_node;
		_InvertNode invert_node;

		int cnt = tmp_info.size();
		if(cnt == 0)
		{
			return;
		}

		invert_vec.reserve(cnt);
		for(int i = 0; i < cnt; i++)
		{
			if(i == 0)
			{
				// ����index_node
				index_node.m_nID = tmp_info[i].m_nPid;
				index_node.m_nOffset = i;
				index_node.m_nCount = 0;
				index_node.m_nTotalWeight = 0;
			}

			if(index_node.m_nID != tmp_info[i].m_nPid)
			{
				index_node.m_nTotalWeight = (int)(sqrt((float)index_node.m_nTotalWeight) * 1000);
				index_map[index_node.m_nID] = index_node;

				// ����index_node
				index_node.m_nID = tmp_info[i].m_nPid;
				index_node.m_nOffset = i;
				index_node.m_nCount = 0;
				index_node.m_nTotalWeight = 0;
			}

			// handle index
			index_node.m_nTotalWeight += tmp_info[i].m_nWeight * tmp_info[i].m_nWeight;
			index_node.m_nCount += 1;

			// handle invert
			invert_node.copy(tmp_info[i]);
			invert_vec.push_back(invert_node);
		}

		// ��β
		index_node.m_nTotalWeight = (int)(sqrt((float)index_node.m_nTotalWeight) * 1000);
		index_map[index_node.m_nID] = index_node;
	}

	// ��index_mapд���ļ���Ϊ�˺;ɰ汾���ּ��ݣ�û�����л�
	void writeIndex()
	{
		FILE *fp_index = fopen(index_file_name.c_str(), "wb");
		for(tr1::unordered_map<int, IndexNode>::iterator it = index_map.begin();
			it != index_map.end(); ++it)
		{
			fwrite(&(it->second), sizeof(IndexNode), 1, fp_index);
		}

		fclose(fp_index);
		fp_index = NULL;
	}

	// ��invert_vecд���ļ�
	void writeInvert()
	{
		FILE *fp_invert = fopen(invert_file_name.c_str(), "wb");
		fwrite(&invert_vec[0], sizeof(_InvertNode), invert_vec.size(), fp_invert);

		fclose(fp_invert);
		fp_invert = NULL;
	}

	// ���ļ������ݣ��������ű�
	bool load(const char *index_fn, const char *invert_fn)
	{
		// �ж��ļ��Ƿ����
		if(access(index_fn, 0) != 0 || access(invert_fn, 0) != 0)
			return false;

		// �����ļ���
		index_file_name = index_fn;
		invert_file_name = invert_fn;

		// ��ջ���
		clear();

		// ���������ļ�
		readIndex();

		if(!is_cached)
		{
			fp = fopen(invert_file_name.c_str(), "rb");
		}
		else
		{
			readInvert();	
		}

		return true;
	}

	// ���ļ�����index_map
	void readIndex()
	{
		int fd = open(index_file_name.c_str(), O_RDONLY);
		struct stat st;
		fstat(fd, &st); /* ȡ���ļ���С */
		void *start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if(start == MAP_FAILED) /* �ж��Ƿ�ӳ��ɹ� */
		{
			return;
		}

		IndexNode *p = (IndexNode *)start;
		int cnt = st.st_size / sizeof(IndexNode);
		for(int i = 0; i < cnt; i++)
		{
			if(p->m_nCount != 0)
			{
				index_map[p->m_nID] = *p;
			}
			p++;
		}

		munmap(start, st.st_size); /* ���ӳ�� */
		p = NULL;
		close(fd);
	}

	// ���ļ�����invert_vec
	void readInvert()
	{
		FILE *fp_invert = fopen(invert_file_name.c_str(), "rb");
		
		fseek(fp_invert, 0, SEEK_END);
		int cnt = ftell(fp_invert) / sizeof(_InvertNode);

		fseek(fp_invert, 0, SEEK_SET);
		invert_vec.reserve(cnt);
		fread(&invert_vec[0], sizeof(_InvertNode), cnt, fp_invert);

		fclose(fp_invert);
		fp_invert = NULL;
	}

	/*
	 * ��ȡĳ��������Ӧ�ĵ���
	 */
	inline bool getPostList(int index, vector<_InvertNode> &temp_vec, _InvertNode* &start, int &cnt)
	{
		// ���vec
		temp_vec.clear();

		// ��������������Ч��
		if (!has_key(index))
		{
			cnt = 0;
			start = NULL;
			return false;
		}

		cnt = index_map[index].m_nCount;

		// ������˻���
		if (is_cached)
		{
			int addr = index_map[index].m_nOffset;
			start = &(invert_vec[addr]);
		}
		else
		{
			// ��Ӳ���ж�ȡ
			if(!readInvertPart(index_map[index], temp_vec))
			{
				return false;
			}
			start = &(temp_vec[0]);
		}

		return true;
	}

	// ��ʹ������Ϣ���ڴ���Ҳ���Ƶ�result vec
	bool getPostList(int index, vector<_InvertNode> &result_vec)
	{
		// ���vec
		result_vec.clear();

		// ��������������Ч��
		if (!has_key(index))
		{
			return false;
		}

		int cnt = index_map[index].m_nCount;
		// ������˻���
		if (is_cached)
		{
			int addr = index_map[index].m_nOffset;
			result_vec.insert(result_vec.end(), invert_vec.begin() + addr,
				invert_vec.begin() + addr + cnt);
		}
		else
		{
			// ��Ӳ���ж�ȡ
			return readInvertPart(index_map[index], result_vec);
		}

		return true;
	}

	// �ӵ����ж�ȡindex_node��Ӧ��һ�����ݣ�����vec��
	inline bool readInvertPart(IndexNode &index_node, vector<_InvertNode> &vecInvert)
	{
		__int64 nAddr = index_node.m_nOffset * sizeof(_InvertNode);
		int cnt = index_node.m_nCount;

		if(fseek(fp, nAddr, 0) == -1)
		{
			return false;
		}

		vecInvert.resize(cnt);

		int real_cnt = fread(&vecInvert[0], sizeof(_InvertNode), cnt, fp);
		if(real_cnt == -1 || real_cnt != cnt)
		{
			return false;
		}

		return true;
	}


};

typedef InvertTable<InvertNode2, ST_PidKidInfo2> InvertTable2;

} //-------------end of namespace invert_table
#endif  //----end of INVERT_TABLE_H_
