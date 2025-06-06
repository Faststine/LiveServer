#pragma once

#include <vector>
#include <sys/epoll.h>
#include <unordered_map>
#include <functional>
#include <queue>
#include <mutex>
#include "Event.h"
#include "PipEvent.h"
#include "TimingWheel.h"

namespace Live
{
    namespace network
    {
        using EventPtr = std::shared_ptr<Event>;
        using Func = std::function<void()>;
        class EventLoop {
        public:
            EventLoop();
            ~EventLoop();

        public:
            void Loop();
            void Quit();
            void AddEvent(const Event::ptr &event);
            void DelEvent(const Event::ptr &event);
            bool EnableEventWriting(const Event::ptr &event, bool enable);
            bool EnableEventReading(const Event::ptr &event, bool enable);

            // 任务队列相关
            void AssertInLoopThread();  // 是否在同一个循环 
            bool IsInLoopThread() const;
            void RunInLoop(const Func& f);
            void RunInLoop(Func &&f);
        
            void InsertEntry(uint32_t delay, EntryPtr entryPtr);   // 插入entry，设置超时时间
            void RunAfter(double delay, Func &cb);    // 设置延迟多少时间执行
            void RunAfter(double delay, Func &&cb);
            void RunEvery(double interval, Func &cb);// 每隔一段时间就执行一遍
            void RunEvery(double interval, Func &&cb);
        private:
            void RunFunctions();
            void WakeUp();      // 唤醒loop

            bool looping_{false};   // 是否正在循环
            int epoll_fd_ {-1};
            std::vector<struct epoll_event> epoll_events_;  // epoll事件数组
            std::unordered_map<int, Event::ptr> events_;    // fd和event关联

            std::queue<Func> functions_;
            std::mutex lock_;
            PipEvent::ptr pipEvent_;
            TimingWheel wheel_;
        };
    
        
    } // namespace network
    
} // namespace Live
