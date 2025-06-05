#include "network/base/InetAddress.h"
#include "network/base/SocketOpt.h"
#include <iostream>

using namespace Live::network;

/// 客户端
// nc -l 34444模拟监听
void TestClient()
{
    // 创建一个socket
    int sock = SocketOpt::CreateNonblockingTcpSocket(AF_INET);
    if(sock < 0)
    {
        std::cerr << "socket failed.sock" << sock << " errno:" << errno << std::endl;
        return;
    }

    // 服务器地址
    InetAddress server("0.0.0.0:34444");
    SocketOpt opt(sock);
    opt.SetNonBlocking(false);
    auto ret = opt.Connect(server);

    std::cout << "connect ret : " << ret << "   errno :" << errno << std::endl
                << "local: " << opt.GetLocalAddr()->ToIpPort() << std::endl
                << "peer: " << opt.GetPeerAddr()->ToIpPort() << std::endl;
}

// 模拟客户端
// ab -c 1 -n 1 "http://172.17.166.229:34444/"
void TestServer()
{
        // 创建一个socket
    int sock = SocketOpt::CreateNonblockingTcpSocket(AF_INET);
    if(sock < 0)
    {
        std::cerr << "socket failed.sock" << sock << " errno:" << errno << std::endl;
        return;
    }

    // 服务器地址
    InetAddress server("0.0.0.0:34444");
    SocketOpt opt(sock);
    opt.SetNonBlocking(false);

    opt.BindAddress(server);
    opt.Listen();
    InetAddress addr;
    auto ns = opt.Accept(&addr);

    std::cout << "accept ret : " << ns << "   errno :" << errno << std::endl
                << "addr: " << addr.ToIpPort() << std::endl;
}

int main()
{
    TestServer();
    return 0;
}