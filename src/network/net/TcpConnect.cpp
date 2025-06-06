#include "TcpConnect.h"
#include "network/base/Network.h"
#include "unistd.h"

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

void TcpConnect::SetWriteCompleteCallback(const WriteCompleteCallback &cb)
{
    mWriteComCallback = cb;
}
void TcpConnect::SetWriteCompleteCallback(const WriteCompleteCallback &&cb)
{
    mWriteComCallback = std::move(cb);
}
void TcpConnect::Send(std::list<BufferNodePtr> &list)
{
    loop_->RunInLoop([this,&list](){
        SendInLoop(list);
    });
}
void TcpConnect::Send(const char* buffer, int size)
{
    loop_->RunInLoop([this,buffer,size](){
        SendInLoop(buffer, size);
    });
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

void TcpConnect::OnWrite()
{
    if (mClose)
    {
        NETWORK_ERROR << "OnWrite";
        return;
    }

    if (!mIovecList.empty())
    {
        auto ret = ::writev(event_read_fd_, &mIovecList[0], mIovecList.size());
        if (ret > 0)
        {
            if (mIovecList.front().iov_len > ret)
            {
                mIovecList.front().iov_base = (char*)mIovecList.front().iov_base + ret;
                mIovecList.front().iov_len -= ret;
            } else {
                ret -= mIovecList.front().iov_len;
                mIovecList.erase(mIovecList.begin());
            }
            if (mIovecList.empty())
            {
                EnableWriting(false);
                if (mWriteComCallback)
                {
                    mWriteComCallback(std::dynamic_pointer_cast<TcpConnect>(shared_from_this()));
                }
                
            } else {
                if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK )
                {
                    NETWORK_ERROR << "error";
                    OnClose();
                    return;
                }
            }    
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

void TcpConnect::SendInLoop(std::list<BufferNodePtr> &list)
{
    if (mClose)
    {
        NETWORK_ERROR << "SendInLoop";
        return;
    }

    for (auto &e:list)
    {
        struct iovec vec;
        vec.iov_base = (void*)e->addr;
        vec.iov_len = e->size;

        mIovecList.push_back(vec);
    }

    if (!mIovecList.empty())
    {
        EnableWriting(true);
    }
    
    
}
void TcpConnect::SendInLoop(const char* buffer, int size)
{
    if (mClose)
    {
        NETWORK_ERROR << "SendInLoop";
        return;
    }
    int sendLen = 0;
    if (!mIovecList.empty())
    {
        sendLen = ::write(event_read_fd_, buffer, size);
        if (sendLen < 0)
        {
            if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK )
            {
                NETWORK_ERROR << "error";
                OnClose();
                return;
            }
            sendLen = 0;
        }
        size -= sendLen;
    }
    if (size > 0) {
        struct iovec vec;
        vec.iov_base = (char*)buffer + sendLen;
        vec.iov_len = size;

        mIovecList.push_back(vec);
        EnableWriting(true);

    }
    
}
