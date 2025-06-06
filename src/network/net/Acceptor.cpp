#include "Acceptor.h"
#include "network/base/Network.h"

using namespace Live::network;
Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr)
:Event(loop),mInetAddr(addr)
{
    
}

Acceptor::~Acceptor()
{
    Stop();
    if (mSocketOpt != nullptr) {
        delete mSocketOpt;
        mSocketOpt = nullptr;
    }
}

void Acceptor::SetAcceptCallback(const acceptCallback &cb)
{
    mAcceptCallback = cb;
}

void Acceptor::SetAcceptCallback(const acceptCallback &&cb)
{
    mAcceptCallback = std::move(cb);
}


void Acceptor::Start()
{
    loop_->RunInLoop([this]()
    {
        Open();
    });
}

void Acceptor::Stop()
{
    loop_->DelEvent(std::dynamic_pointer_cast<Acceptor>(shared_from_this()));
}

void Acceptor::OnRead()
{
    if (mSocketOpt == nullptr) {
        return;
    }

    while (true)
    {
        InetAddress addr;
        auto sock = mSocketOpt->Accept(&addr);
        if (sock >= 0)
        {
            if (mAcceptCallback)
            {
                mAcceptCallback(sock, addr);
            }
            
        } else {
            if (errno != EINTR && errno != EAGAIN)
            {
                NETWORK_ERROR << "Accept error" << errno;
                OnClose();
            } 
            break;    
        }   

    }
}

void Acceptor::OnError(const std::string &msg)
{
    NETWORK_ERROR << "accept error" << msg;
    OnClose();
}

void Acceptor::OnClose()
{
    Stop();
    Open();
}

void Acceptor::Open()
{
    if (event_read_fd_ > 0) {
        ::close(event_read_fd_);
        event_read_fd_ = -1;
    }
    if (mInetAddr.IsIpv6()) {
        event_read_fd_ = SocketOpt::CreateNonblockingTcpSocket(AF_INET6);
    } else {
        event_read_fd_ = SocketOpt::CreateNonblockingTcpSocket(AF_INET);
    }
    if (event_read_fd_ < 0) {
        NETWORK_ERROR << "ipv4 Open failed" << event_read_fd_ << "convert failed";
    }
    if (mSocketOpt != nullptr) {
        delete mSocketOpt;
        mSocketOpt = nullptr;
    }
    loop_->AddEvent(std::dynamic_pointer_cast<Acceptor>(shared_from_this()));
    mSocketOpt = new SocketOpt(event_read_fd_);
    mSocketOpt->SetReuseAddr(true);
    mSocketOpt->SetReusePort(true);
    mSocketOpt->BindAddress(mInetAddr);
    mSocketOpt->Listen();
}