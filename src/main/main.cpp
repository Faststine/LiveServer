#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{
    // 启动定时任务
    while(1)
    {
        std::cout << "Hello world" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}