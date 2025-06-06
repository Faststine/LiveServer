#include "EventLoopThreadPool.h"
#include <pthread.h>

using namespace Live::network;
namespace {
    void bind_cpu(std::thread &t, int n)
    {
        cpu_set_t cpu;
        CPU_ZERO(&cpu);
        CPU_SET(n, &cpu);
        pthread_setaffinity_np(t.native_handle(),sizeof(cpu), &cpu);
    }
}
EventLoopThreadPool::EventLoopThreadPool(int thread_num, int start, int cpus)
{
    if (thread_num <= 0) {
        thread_num = 1;
    } 

    for (size_t i = 0; i < thread_num; i++)
    {
        threads_.emplace_back(std::make_shared<EventLoopThread>());
        if (cpus > 0) {
            int n = (start+i)%cpus;
            bind_cpu(threads_.back()->Thread(), n);
        }
    }
    
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}


std::vector<EventLoop*> EventLoopThreadPool::GetLoops()
{
    std::vector<EventLoop*> result;
    for (size_t i = 0; i < threads_.size(); i++)
    {
        result.emplace_back(threads_[i]->Loop()); 
    }
    return result;
    
}

EventLoop *EventLoopThreadPool::GetNextLoop()
{
    int index = loop_index_;
    loop_index_++;
    return threads_[index%threads_.size()]->Loop();
}

int EventLoopThreadPool::Size()
{
    return threads_.size();
}

void EventLoopThreadPool::Start()
{
    for (size_t i = 0; i < threads_.size(); i++)
    {
        threads_[i]->Run();
    }
    
}