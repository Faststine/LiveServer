#include "network/net/Acceptor.h"
#include "network/net/Event.h"
#include "network/net/EventLoop.h"
#include "network/net/EventLoopThread.h"
#include "network/net/TcpConnect.h"

#include <iostream>
const char *http_response = "HTTP/1.0 200 OK\r\nServer: tmms\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
using namespace Live::network;
EventLoopThread eventLoop_thread;
std::thread th;
int main() {
    eventLoop_thread.Run();
    EventLoop *loop = eventLoop_thread.Loop();

    if (loop)
    {
        InetAddress server("172.17.166.229:34444");
        std::shared_ptr<Acceptor> acceptor = std::make_shared<Acceptor>(loop, server);
        acceptor->SetAcceptCallback([&loop, &server](int fd,const InetAddress &addr)
        {
            std::cout << "host:" <<addr.ToIpPort() << std::endl;
            TcpConnectionPtr connection = std::make_shared<TcpConnect>(loop,fd,server,server);
            connection->SetRecvMsgCallback([](const TcpConnectionPtr & con,MsgBuffer & buf){
                std::cout << "recv msg:" <<buf.Peek() << std::endl;
                buf.RetrieveAll();
                con->Send(http_response,strlen(http_response));
            });

            connection->SetWriteCompleteCallback([&loop](const TcpConnectionPtr & con){
                std::cout << "write msg:" <<con->PeerAddr().ToIpPort() << std::endl;
                loop->DelEvent(con);
                con->ForceClose();
            });
        });
        acceptor->Start();
        while (1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
    }
    return 0;
    
}