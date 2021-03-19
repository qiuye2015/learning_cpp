/*************************************************************************
  > File Name: threadPool.c
  > Author: fjp
  > Mail: fjp@baidu.com
  > Created Time: Sat Jul 11 19:54:33 2020
 ************************************************************************/

/*********************************线程池*********************************
 * 1. 任务队列
 * 2. 执行队列
 * 3. 管理组件
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>//sleep
#include<memory.h>
#include<pthread.h>
// 链表头插法
#define LL_INSERT(item, list) do {			\
	item->prev = NULL;						\
	item->next = list;						\
	if (list != NULL) list->prev = item;	\
	list = item;							\
} while(0)

// 链表删除指定元素
#define LL_REMOVE(item, list) do {							\
	if (item->prev != NULL) item->prev->next = item->next;	\
	if (item->next != NULL) item->next->prev = item->prev;	\
	if (list == item) list = item->next;					\
	item->prev = item->next = NULL;							\
} while(0)

// 1. 任务队列
typedef struct NJOB{

	void (*func)(void *arg);
	void *user_data;

	struct NJOB *next;
	struct NJOB *prev;

} nJob;

struct NMANAGER;//
// 2. 执行队列
typedef struct NWORKER{

	pthread_t threadid;
	int terminate;//终止标识
	struct NMANAGER *pool; //

	struct NWORKER *next;
	struct NWORKER *prev;

} nWorker;

// 3. 管理组件
typedef struct NMANAGER{

	nJob *jobs;
	nWorker *workers;

	pthread_mutex_t mtx;
	pthread_cond_t cond;

} nThreadPool;

static void *nThreadCallback(void *arg){
	nWorker *worker = (nWorker*)arg;
	while(1){
		pthread_mutex_lock(&worker->pool->mtx);
		while(worker->pool->jobs == NULL){
			if (worker->terminate) break;
			pthread_cond_wait(&worker->pool->cond, &worker->pool->mtx);
		}
		if (worker->terminate) {
			pthread_mutex_unlock(&worker->pool->mtx);
			break;
		}
		nJob *job = worker->pool->jobs;
		LL_REMOVE(job,worker->pool->jobs);

		pthread_mutex_unlock(&worker->pool->mtx);//条件等待之前加锁，条件等待之后解锁
		job->func(job->user_data);//主要处理逻辑
	}
	free(worker);
	pthread_exit(NULL);
}

// api
int nThreadPoolCreate(nThreadPool *pool, int numWorker){
	if (pool == NULL) return -1;
	if (numWorker < 1) numWorker = 1;

	//malloc --> memset 避免脏数据
	memset(pool, 0, sizeof(nThreadPool));

	// condition
	pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
	memcpy(&pool->cond, &blank_cond, sizeof(pthread_cond_t));

	// mutex
	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
	memcpy(&pool->mtx, &blank_mutex, sizeof(pthread_mutex_t));

	// worker
	int i= 0;
	for(i = 0; i < numWorker; i++) {
		nWorker *worker = (nWorker*)malloc(sizeof(nWorker));
		if (worker == NULL) {
			perror("malloc");
			return -2;
		}
		memset(worker, 0, sizeof(nWorker));
		worker->pool = pool;//
		int ret = pthread_create(&worker->threadid, NULL, nThreadCallback, worker);
		if (ret){
			perror("pthread_create");
			free(worker);
			return -3;
		}

		LL_INSERT(worker, pool->workers);
	}
	return 0;
}

//
void nThreadPoolDestory(nThreadPool *pool){
	nWorker *worker = NULL;
	for(worker=pool->workers; worker != NULL;worker = worker->next) {
		worker->terminate = 1;
	}
	pthread_mutex_lock(&pool->mtx);
	pthread_cond_broadcast(&pool->cond);
	pthread_mutex_unlock(&pool->mtx);
}

//
void nThreadPoolPushTask(nThreadPool *pool, nJob *job){
	pthread_mutex_lock(&pool->mtx);

	LL_INSERT(job, pool->jobs);
	pthread_cond_signal(&pool->cond);

	pthread_mutex_unlock(&pool->mtx);
}

#if 1	//DEBUG
void print(void *arg){
	printf("%d\n",*(int*)(arg));
}
int main(){
	int i=0;
	nThreadPool *pool;
	pool = (nThreadPool*)malloc(sizeof(nThreadPool));
	nThreadPoolCreate(pool, 10);
	for(i=0;i<10;i++){
		nJob job;
		job.func = &print;
		job.user_data = (void*)(&i);
		nThreadPoolPushTask(pool,&job);
		sleep(1);
	}
	sleep(15);
	nThreadPoolDestory(pool);
}
#endif
// 如何增加减少线程数
// 减少增加的策略是什么
