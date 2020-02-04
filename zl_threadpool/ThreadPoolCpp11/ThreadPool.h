#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <type_traits>//std::result_of
#include <utility>
#include <vector>
#include <iostream>

// C++11 版的 线程池
namespace zl
{
    class ThreadsGuard
    {
    public:
        ThreadsGuard(std::vector<std::thread>& v)
            : threads_(v)
        {
            std::cout<<"ThreadGuard "<<threads_.size()<<std::endl;
        }

        ~ThreadsGuard()
        {
            std::cout<<"~ThreadGuard "<<threads_.size()<<std::endl;
            for (size_t i = 0; i != threads_.size(); ++i)
            {
                if (threads_[i].joinable())
                {
                    threads_[i].join();//等待线程结束
                }
            }
        }
    private:
        ThreadsGuard(ThreadsGuard&& tg) = delete;
        ThreadsGuard& operator = (ThreadsGuard&& tg) = delete;

        ThreadsGuard(const ThreadsGuard&) = delete;
        ThreadsGuard& operator = (const ThreadsGuard&) = delete;
    public:
        std::vector<std::thread>& threads_;
    };


    class ThreadPool
    {
    public:
        typedef std::function<void()> task_type;

    public:
        explicit ThreadPool(int n = 0);

        ~ThreadPool()
        {
            std::cout<<"~ThreadPool"<<std::endl;
            stop();
            cond_.notify_all();
        }

        void stop()
        {
            stop_.store(true, std::memory_order_release);
        }

        template<class Function, class... Args>
        std::future<typename std::result_of<Function(Args...)>::type> add(Function&&, Args&&...);

    private:
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator = (ThreadPool&&) = delete;
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator = (const ThreadPool&) = delete;

    private:
        std::atomic<bool> stop_;
        std::mutex mtx_;
        std::condition_variable cond_;

        std::queue<task_type> tasks_;
        std::vector<std::thread> threads_;
        zl::ThreadsGuard tg_;//在线程池析构时调用join等待 threads_中的线程结束，tg_中线程与threads_本质是一个
    };


    inline ThreadPool::ThreadPool(int n)
        : stop_(false)
        , tg_(threads_)
    {
        std::cout<<"ThreadPool"<<std::endl;
        int nthreads = n;
        if (nthreads <= 0)
        {
            nthreads = std::thread::hardware_concurrency();
            nthreads = (nthreads == 0 ? 2 : nthreads);
        }

        for (int i = 0; i != nthreads; ++i)
        {
            threads_.push_back(std::thread([this]{
                while (!stop_.load(std::memory_order_acquire))
                {
                    task_type task;
                    {
                        std::unique_lock<std::mutex> ulk(this->mtx_);
                        this->cond_.wait(ulk, [this]{ return stop_.load(std::memory_order_acquire) || !this->tasks_.empty(); });
                        if (stop_.load(std::memory_order_acquire))
                            return;
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }
                    task();
                }
            }));
            std::cout<<"tg_ "<<tg_.threads_.size()<<std::endl;
        }
    }

    template<class Function, class... Args>
    std::future<typename std::result_of<Function(Args...)>::type>
        ThreadPool::add(Function&& fcn, Args&&... args)
    {
        typedef typename std::result_of<Function(Args...)>::type return_type;
        typedef std::packaged_task<return_type()> task;

        auto t = std::make_shared<task>(std::bind(std::forward<Function>(fcn), std::forward<Args>(args)...));
        auto ret = t->get_future();
        {
            std::lock_guard<std::mutex> lg(mtx_);
            if (stop_.load(std::memory_order_acquire))
                throw std::runtime_error("thread pool has stopped");
            tasks_.emplace([t]{(*t)(); });
        }
        cond_.notify_one();
        return ret;
    }
}

#endif  /* THREAD_POOL_H */