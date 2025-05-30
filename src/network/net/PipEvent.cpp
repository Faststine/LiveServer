#include "PipEvent.h"
#include "network/base/Network.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

using namespace tmms::network;
PipEvent::PipEvent(EventLoop *loop)
:Event(loop)
{
    int fd[2] = {0,};
    auto ret = ::pipe2(fd, O_NONBLOCK);
    if(ret < 0) {
        NETWORK_ERROR << "pipe2 open failed";
        exit(-1);
    }
    event_read_fd_ = fd[0];
    write_fd_ = fd[1];
}

PipEvent::~PipEvent()
{
    ::close(write_fd_);
    write_fd_ = -1;
}

void PipEvent::OnRead()
{
    int64_t tmp;
    auto ret = ::read(event_read_fd_, &tmp, sizeof(tmp));
    if(ret < 0)
    {
        NETWORK_ERROR << "pipe read error.error: " << errno;
    }
    std::cout << "pipe read tmp : " << tmp << std::endl;
}
void PipEvent::OnClose()
{
    if (write_fd_ > 0) {
        ::close(write_fd_);
        write_fd_ = -1;
    }
}
void PipEvent::OnError(const std::string &msg)
{
    NETWORK_ERROR << "pipe2 OnError";
}
void PipEvent::Write(const char *data, size_t len)
{
    ::write(write_fd_, data, len);
}
