#include "network/net/EventLoopThread.h"
#include "network/net/EventLoop.h"
#include "network/net/PipEvent.h"
#include "base/TTime.h"
#include "network/net/EventLoopThreadPool.h"
#include <iostream>

using namespace Live::network;
using namespace Live::base;

EventLoopThread eventLoop_thread;   // 构造器自动创建一个线程
std::thread th;

void writeDataPeriodically(std::shared_ptr<PipEvent> pipeEvent) {
    while (true) {
        int64_t now = Live::base::TTime::NowMS();
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
        PipEvent::ptr pipEvent = std::make_shared<PipEvent>(loop);
        loop->AddEvent(pipEvent);
        int64_t test = 123456;
        pipEvent->Write((const char*)&test, sizeof(test));

        th = std::thread(writeDataPeriodically, pipEvent);
        th.detach();
        while(1)
        {

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}
 

void TestEventLoopThreadPool()
{
    EventLoopThreadPool pool(2,0,2);
    pool.Start();

    std::cout << "thread id:"<<std::this_thread::get_id() << std::endl;
    std::vector<EventLoop*> loopList = pool.GetLoops();
    for (size_t i = 0; i < loopList.size(); i++) {
        loopList[i]->RunInLoop([eLoop = loopList[i]](){
            std::cout << "loop:"<<eLoop<<" thread id:"<<std::this_thread::get_id() << std::endl;
        });
    }

    // EventLoop* loop = pool.GetNextLoop();
    // std::cout << "loop:"<<loop<<std::endl;
    // loop = pool.GetNextLoop();
    // std::cout << "loop:"<<loop<<std::endl;
}

void TestEventLoopTimingWheel()
{
    EventLoopThreadPool pool(2, 0, 2);      // 2个线程，0cpu开始，2个cpu跑
    pool.Start();
    EventLoop* loop = pool.GetNextLoop();
    std::cout << "loop : " << loop << std::endl; 

    // 设置任务
    loop->RunAfter(1, [](){
        std::cout << "run after 1s now : " << Live::base::TTime::Now() << std::endl;
    });
    std::cout << "loop1 : " << loop << std::endl; 
    loop->RunEvery(2, [](){
        std::cout << "run every 2s now : " << Live::base::TTime::Now() << std::endl;
    });
    std::cout << "loop2 : " << loop << std::endl; 
    loop->RunEvery(3, [](){
        std::cout << "run every 3s now : " << Live::base::TTime::Now() << std::endl;
    });
    std::cout << "loop 3: " << loop << std::endl; 
    while(1)  // 退出函数线程池就析构了
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    //TestEventLoopThread();

    //TestEventLoopThreadPool();

    TestEventLoopTimingWheel();
    while(1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}