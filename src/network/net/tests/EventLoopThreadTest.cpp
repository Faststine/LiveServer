#include "network/net/EventLoopThread.h"
#include "network/net/EventLoop.h"
#include "network/net/PipEvent.h"
#include <iostream>

using namespace tmms::network;

EventLoopThread eventLoop_thread;   // 构造器自动创建一个线程
std::thread th;

void writeDataPeriodically(std::shared_ptr<PipEvent> pipeEvent) {
    while (true) {
        int64_t now = tmms::base::TTime::NowMS();
        pipeEvent->Write((const char*)&now, sizeof(now));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void TestEventLoopThread()
{
    eventLoop_thread.Run(); // 解除loop前的阻塞，eventloop开始循环
    EventLoop* loop = eventLoop_thread.Loop();

    if(loop)  // 这个loop是内部局部loop的引用，如果内部的eventloop停止了就变nullptr了，实现控制
    {
        std::cout << "loop : " << loop << std::endl;
        PipEvent::ptr pipeEvent = std::make_shared<PipEvent>(loop);
        loop->AddEvent(pipeEvent);
        int64_t test = 123456;
        pipeEvent->Write((const char*)&test, sizeof(test));
        while(1)
        {

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}
 
int main()
{
    TestEventLoopThread();
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}