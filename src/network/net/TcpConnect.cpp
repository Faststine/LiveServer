#include "TcpConnect.h"
#include "network/base/Network.h"

using namespace Live::network;

TcpConnect::TcpConnect(EventLoop*loop, int socket_fd, const InetAddress &localAddr, const InetAddress &peerAddr)
:Connection(loop, socket_fd, localAddr, peerAddr)
{

}

TcpConnect::~TcpConnect()
{
    OnClose();
}



void TcpConnect::SetCloseCallback(const CloseConnectionCallback &cb)
{
    mCloseCb = cb;
}

void TcpConnect::SetCloseCallback(CloseConnectionCallback &&cb){
    mCloseCb = std::move(cb);
}

void TcpConnect::SetRecvMsgCallback(const MessageCallback& cb)
{
    mMessageCallback = cb;
}

void TcpConnect::SetRecvMsgCallback(MessageCallback&& cb)
{
     mMessageCallback = std::move(cb);
}

void TcpConnect::OnClose()
{
    loop_->AssertInLoopThread();
    if (mClose == true)
    {
       mCloseCb(std::dynamic_pointer_cast<TcpConnect>(shared_from_this()));
    }
    mClose = true;
}

void TcpConnect::OnRead()
{
    if (mClose == true) {
       NETWORK_ERROR << "host:"<<mPeerAddr.ToIpPort() << "has closed";
    }
    while (true)
    {
        int err = 0;
        auto ret =  mMessageBuffer.ReadFd(event_read_fd_, &err);
        if (ret > 0)
        {
             if (mMessageCallback)
             {
                mMessageCallback(std::dynamic_pointer_cast<TcpConnect>(shared_from_this()),mMessageBuffer);
             }
             
        } else if (ret == 0) {
           OnClose();
           break;
        } else {
            if (err != EINTR && err != EAGAIN && err != EWOULDBLOCK )
            {
                NETWORK_ERROR << "error";
            }
            break;
        }
    } 
}

void TcpConnect::ForceClose()
{
    loop_->RunInLoop([this](){
        OnClose();
    });
}

void TcpConnect::OnError(const std::string &msg)
{
    NETWORK_ERROR << "error:" <<mPeerAddr.ToIpPort()<<"msg"<< msg;
    OnClose();
}
