#include "Connection.h"

using namespace Live::network;
Connection::Connection(EventLoop*loop, int fd, const InetAddress &localAddr, const InetAddress &peerAddr)
:Event(loop, fd), mLocalAddr(localAddr), mPeerAddr(peerAddr)
{

}

void Connection::SetLocalAddr(const InetAddress &local)
{
    mLocalAddr = local;
}

void Connection::SetPeerAddr(const InetAddress &peer)
{
    mPeerAddr = peer;
}

const InetAddress &Connection::LocalAddr() const{
    return mLocalAddr;
}

const InetAddress &Connection::PeerAddr() const{
    return mPeerAddr;
}

void Connection::SetContext(int type, const std::shared_ptr<void> &context)
{
    mContext[type] = context;
}

void Connection::SetContext(int type, std::shared_ptr<void> &&context)
{
    mContext[type] = std::move(context);
}


void Connection::ClearContxt(int type)
{
    mContext[type].reset();
}

void Connection::ClearContxt()
{
    mContext.clear();
}


void Connection::SetActiveCallback(const ActiveCallback &callback)
{
    mActiveCb = callback;
}

void Connection::SetActiveCallback(const ActiveCallback &&callback)
{
    mActiveCb = std::move(callback);
}

void Connection::Active()
{
    if (active_.load() == 0)
    {
        loop_->RunInLoop([this]()
        {  
            active_.store(true);
            if (mActiveCb)
            {
                mActiveCb(std::dynamic_pointer_cast<Connection>(shared_from_this()));
            }
                   
        });
    }
    
}

void Connection::Deactive()
{
    active_.store(false);
}