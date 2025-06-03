#pragma once

#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include "EventLoop.h"

namespace Live
{
    namespace network
    {
        class EventLoopThread
        {
        private:
            void StartEventLoop();
        public:
            EventLoopThread(/* args */);
            ~EventLoopThread();
            std::thread &Thread();
            EventLoop* Loop() const;
            using ptr = std::shared_ptr<EventLoopThread>;
        void Run();
        private:
            EventLoop* loop_{nullptr};
            std::thread thread_;
            bool running_{false};
            std::mutex lock_;
            std::condition_variable condition_;
            std::once_flag once_;
            std::promise<int> promise_;
        };
    }
}