/*
 * 倒排表：
 * 1. index总会常驻内存，没有数据的index节点被删掉了，遍历需要注意。
 * 2. invert可选择是否缓存。（默认缓存）
 * 3. create时可选择是否持久化，当不开缓存时，invert总会持久化。
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

//默认情况下，在Linux系统下，fopen和open操作的文件大小不能超过2G
#define _FILE_OFFSET_BITS 64
typedef long long __int64

namespace fjp {

// 四元组
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

// 四元组的InvertNode，去掉indexID变为三元组
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
 * STNode的排序方法
 * 被invert_table_creater调用
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
 * InvertNode的排序方法
 * 注意：倒排create或load过程中不会对InvertNode进行排序
 *       传入的vec或倒排文件，已经是有序的

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
 * 倒排表
 */
template <typename _InvertNode, typename _ST_PidKidInfo>
class InvertTable
{
public:
	// 缓存开关
	bool is_cached;
	// 当不启用缓存时，打开invert文件的指针
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

	// 建立倒排表，设置输出目录
	void create(vector<_ST_PidKidInfo> &vec, const char *dir, bool is_persistent)
	{
		// 设置倒排文件名
		index_file_name = string(dir) + "/index.idx";
		invert_file_name = string(dir) + "/invert.idx";

		// 清空缓存
		clear();

		// 内存中建立倒排表
		createInvertTable(vec);

		// 持久化，把倒排结构写入文件
		if(is_persistent)
		{
			writeIndex();
			writeInvert();
		}
		else if(!is_cached)
		{
			writeInvert();
		}

		// 如果不缓存，打开倒排文件，清空倒排vec
		if(!is_cached)
		{
			fp = fopen(invert_file_name.c_str(), "rb");
			invert_vec.clear();
		}
	}

	/*
	 * 从vector中读取数据，建立倒排表
	 * 注意：这个vector至少要按<index_id>排过序
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
				// 重置index_node
				index_node.m_nID = tmp_info[i].m_nPid;
				index_node.m_nOffset = i;
				index_node.m_nCount = 0;
				index_node.m_nTotalWeight = 0;
			}

			if(index_node.m_nID != tmp_info[i].m_nPid)
			{
				index_node.m_nTotalWeight = (int)(sqrt((float)index_node.m_nTotalWeight) * 1000);
				index_map[index_node.m_nID] = index_node;

				// 重置index_node
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

		// 收尾
		index_node.m_nTotalWeight = (int)(sqrt((float)index_node.m_nTotalWeight) * 1000);
		index_map[index_node.m_nID] = index_node;
	}

	// 将index_map写入文件，为了和旧版本保持兼容，没有序列化
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

	// 将invert_vec写入文件
	void writeInvert()
	{
		FILE *fp_invert = fopen(invert_file_name.c_str(), "wb");
		fwrite(&invert_vec[0], sizeof(_InvertNode), invert_vec.size(), fp_invert);

		fclose(fp_invert);
		fp_invert = NULL;
	}

	// 从文件读数据，建立倒排表
	bool load(const char *index_fn, const char *invert_fn)
	{
		// 判断文件是否存在
		if(access(index_fn, 0) != 0 || access(invert_fn, 0) != 0)
			return false;

		// 设置文件名
		index_file_name = index_fn;
		invert_file_name = invert_fn;

		// 清空缓存
		clear();

		// 读入索引文件
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

	// 从文件读入index_map
	void readIndex()
	{
		int fd = open(index_file_name.c_str(), O_RDONLY);
		struct stat st;
		fstat(fd, &st); /* 取得文件大小 */
		void *start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if(start == MAP_FAILED) /* 判断是否映射成功 */
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

		munmap(start, st.st_size); /* 解除映射 */
		p = NULL;
		close(fd);
	}

	// 从文件读入invert_vec
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
	 * 读取某个索引对应的倒排
	 */
	inline bool getPostList(int index, vector<_InvertNode> &temp_vec, _InvertNode* &start, int &cnt)
	{
		// 清空vec
		temp_vec.clear();

		// 检查索引坐标的有效性
		if (!has_key(index))
		{
			cnt = 0;
			start = NULL;
			return false;
		}

		cnt = index_map[index].m_nCount;

		// 如果开了缓存
		if (is_cached)
		{
			int addr = index_map[index].m_nOffset;
			start = &(invert_vec[addr]);
		}
		else
		{
			// 从硬盘中读取
			if(!readInvertPart(index_map[index], temp_vec))
			{
				return false;
			}
			start = &(temp_vec[0]);
		}

		return true;
	}

	// 即使倒排信息在内存中也复制到result vec
	bool getPostList(int index, vector<_InvertNode> &result_vec)
	{
		// 清空vec
		result_vec.clear();

		// 检查索引坐标的有效性
		if (!has_key(index))
		{
			return false;
		}

		int cnt = index_map[index].m_nCount;
		// 如果开了缓存
		if (is_cached)
		{
			int addr = index_map[index].m_nOffset;
			result_vec.insert(result_vec.end(), invert_vec.begin() + addr,
				invert_vec.begin() + addr + cnt);
		}
		else
		{
			// 从硬盘中读取
			return readInvertPart(index_map[index], result_vec);
		}

		return true;
	}

	// 从倒排中读取index_node对应的一段数据，存入vec中
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
