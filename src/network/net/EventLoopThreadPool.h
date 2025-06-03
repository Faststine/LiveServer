#pragma once

#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "base/NonCopyable.h"

namespace Live
{
    namespace network
    {
        class EventLoopThreadPool : public base::NonCopyable
        {
        private:
            std::vector<EventLoopThread::ptr> threads_;
            std::atomic_int32_t loop_index_{0};
        public:
            EventLoopThreadPool(int thread_num, int start = 0, int cpus = 4);
            ~EventLoopThreadPool();

            std::vector<EventLoop*> GetLoops();
            EventLoop *GetNextLoop();
            int Size();
            void Start();
        };
    }
}