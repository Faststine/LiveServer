#include "EventLoopThread.h"

using namespace tmms::network;

EventLoopThread::EventLoopThread()
:thread_([this](){StartEventLoop();})   
{

}

EventLoopThread::~EventLoopThread()
{
}

void EventLoopThread::Run()
{
    std::call_once(once_, [this](){
        {
        std::lock_guard<std::mutex> lk(lock_);
        running_ = true;
        condition_.notify_all();
        }
        auto f = promise_.get_future();
        f.get();
    });
}

EventLoop* EventLoopThread::Loop() const
{
    return loop_;
}

void EventLoopThread::StartEventLoop()
{
    EventLoop loop;

    std::unique_lock<std::mutex> lk(lock_);
    condition_.wait(lk,[this](){return running_;});
    loop_ =&loop; 
    promise_.set_value(1);
    loop.Loop();
    loop_ = nullptr;
}
