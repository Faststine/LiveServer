#include "Event.h"
#include "EventLoop.h"
#include <unistd.h>

using namespace Live::network;

Event::Event()
:event_read_fd_(-1), loop_(nullptr)
{
}

Event::Event(EventLoop *loop)
:loop_(loop)
{
}

Event::Event(EventLoop *loop, int fd)
:event_read_fd_(fd), loop_(loop)
{
}

Event::~Event()
{
    Close();
}
 
void Event::OnRead()
{
}

void Event::OnWrite()
{
}

void Event::OnClose()
{
}

void Event::OnError(const std::string &msg)
{
}

/// @brief 使能写事件，直接调用的loop
/// @param enable 
/// @return 
bool Event::EnableWriting(bool enable)
{
    return loop_->EnableEventWriting(shared_from_this(),enable);
}

bool Event::EnableReading(bool enable) 
{
    return loop_->EnableEventReading(shared_from_this(),enable);
}

int Event::Fd() const
{
    return event_read_fd_;
}

void Event::Close()
{
    if(event_read_fd_ > 0)
    {
        ::close(event_read_fd_);
        event_read_fd_ = -1;
    }
}
