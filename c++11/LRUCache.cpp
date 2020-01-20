/*************************************************************************
    > File Name: LRUCache.hpp
    > Author: fjp
    > Mail: fjp@dangdang.com 
    > Created Time: Tue 07 Jan 2020 01:56:45 PM CST
g++ -std=c++11 LRUCache.cpp -lpthread -g
 ************************************************************************/
//#pragma once
#include <iostream>
#include <unordered_map>
#include <list>
#include <thread>
#include <mutex>

using namespace std;

class LRUCache{
private:
	int cap;
	// 双链表：装着 (key, value) 元组
	list<pair<int,int>> cache;
	// 哈希表：key 映射到 (key, value) 在 cache 中的位置
	unordered_map<int,list<pair<int,int>>::iterator> map;
	mutex mtx;//互斥量
	
public:
	LRUCache(int capacity){
		this->cap = capacity;
	}
	void put(int key,int value){
		lock_guard<mutex> lck(mtx);
		/* 要先判断 key 是否已经存在 */
		auto iter = map.find(key);
		/* key 存在，更改 value 并换到队头 */
		if(iter != map.end()){
			cache.erase(iter->second);
		}
		cache.push_front(make_pair(key,value));
		map[key]=cache.begin();
		/*cache 已满，删除尾部的键值对腾位置*/
		if(map.size() > cap){
			auto lastKey = cache.rbegin()->first;
			cache.pop_back();
			map.erase(lastKey);
		}
	}
	bool get(int key,int &value){
		lock_guard<mutex> lck(mtx);
		/* 要先判断 key 是否已经存在 */
		auto iter = map.find(key);
		// 访问的 key 不存在
		if(iter == map.end()){
			return false;
		}
		//key 存在，把 (k, v) 换到队头
		//将cache中it->second的元素（即最后一个）移动到cache.begin(队头)
		cache.splice(cache.begin(),cache,iter->second);
		// 不需要更新 (key, value) 在 cache 中的位置
		value = iter->second->second;
		return true;
	}
};
void test();
LRUCache* cache = new LRUCache(2);

int main(){
	const int N=1;
	thread threads[N];
	for(int i = 0;i<N;i++){
		threads[i] = thread(test);
	}
	for(auto& t : threads){
		t.join();
	}
	return 0;
}
void test(){
	int val=0;
	cache->put(1,1);
	cache->put(2,2);
	if(cache->get(1,val)){
		cout<<val<<endl;
	}else{
		cout<<"no get "<<1<<endl;
	}
	cache->put(3,3);
	if(cache->get(2,val)){
		cout<<val<<endl;
	}else{
		cout<<"no get"<<2<<endl;
	}
	cache->put(4,4);
	if(cache->get(1,val)){
		cout<<val<<endl;
	}else{
		cout<<"no get"<<1<<endl;
	}
	if(cache->get(3,val)){
		cout<<val<<endl;
	}else{
		cout<<"no get"<<3<<endl;
	}
	if(cache->get(4,val)){
		cout<<val<<endl;
	}else{
		cout<<"no get"<<4<<endl;
	}
}
