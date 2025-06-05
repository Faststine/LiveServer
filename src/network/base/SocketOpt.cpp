#include "SocketOpt.h"
#include "Network.h"
#include <arpa/inet.h>  // 包含 sockaddr_in 的定义

using namespace Live::network;

SocketOpt::SocketOpt(int sock, bool ipV6)
:mSock(sock), mIsIpv6(ipV6)
{

}

int SocketOpt::CreateNonblockingTcpSocket(int family)
{
    int sock = ::socket(family, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, IPPROTO_TCP);
    if (sock < 0) {
      NETWORK_ERROR << "create tcp socket failed";
    }
    return sock;
}

int SocketOpt::CreateNonblockingUdpSocket(int family)
{
    int sock = ::socket(family, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, IPPROTO_UDP);
    if (sock < 0) {
      NETWORK_ERROR << "create udp socket failed";
    }
    return sock;
}

void SocketOpt::BindAddress(const InetAddress &localaddr)
{
    if (localaddr.IsIpv6()) {
        struct sockaddr_in6 addr;
        localaddr.GetSocketAddr((struct sockaddr*)&addr);
        socklen_t size = sizeof(struct sockaddr);
        ::bind(mSock, (struct sockaddr*)&addr,size);
    } else {
        struct sockaddr_in addr;
        localaddr.GetSocketAddr((struct sockaddr*)&addr);
        socklen_t size = sizeof(struct sockaddr);
        ::bind(mSock, (struct sockaddr*)&addr,size);
    }
}

int SocketOpt::Listen()
{
    return ::listen(mSock, SOMAXCONN);
}

int SocketOpt::Accept(InetAddress *peeraddr)
{
    struct sockaddr_in6 addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    int sock = ::accept4(mSock, (struct sockaddr*)&addr, &len, SOCK_CLOEXEC|SOCK_NONBLOCK);
    if (sock > 0) {
        if (addr.sin6_family == AF_INET) {
            char ip[16] = {0,};
            struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
            ::inet_ntop(AF_INET, &(saddr->sin_addr.s_addr), ip, sizeof(ip));
            peeraddr->SetAddr(ip);
            peeraddr->SetPort(ntohs(saddr->sin_port));
        } else if (addr.sin6_family == AF_INET6) {
            char ip[INET6_ADDRSTRLEN] = {0,};
            ::inet_ntop(AF_INET6, &(addr.sin6_addr), ip, sizeof(ip));
            peeraddr->SetAddr(ip);
            peeraddr->SetPort(ntohs(addr.sin6_port));
            peeraddr->SetIsIpv6(true);
        }
    }

    return sock;
}

int SocketOpt::Connect(const InetAddress &addr)
{
    struct sockaddr_in6 addr_in;
    addr.GetSocketAddr((struct sockaddr*)&addr_in);
    return ::connect(mSock, (struct sockaddr*)&addr_in, sizeof(struct sockaddr_in6));
}

InetAddress::ptr SocketOpt::GetLocalAddr()
{
    struct sockaddr_in6 addr_in;
    socklen_t len = sizeof(struct sockaddr_in6);
    ::getsockname(mSock,(struct sockaddr*)&addr_in, &len);
    InetAddress::ptr localaddr = std::make_shared<InetAddress>();
    if (addr_in.sin6_family == AF_INET) {
            char ip[16] = {0,};
            struct sockaddr_in *saddr = (struct sockaddr_in *)&addr_in;
            ::inet_ntop(AF_INET, &(saddr->sin_addr.s_addr), ip, sizeof(ip));
            localaddr->SetAddr(ip);
            localaddr->SetPort(ntohs(saddr->sin_port));
        } else if (addr_in.sin6_family == AF_INET6) {
            char ip[INET6_ADDRSTRLEN] = {0,};
            ::inet_ntop(AF_INET6, &(addr_in.sin6_addr), ip, sizeof(ip));
            localaddr->SetAddr(ip);
            localaddr->SetPort(ntohs(addr_in.sin6_port));
            localaddr->SetIsIpv6(true);
        }
    return localaddr;
}

InetAddress::ptr SocketOpt::GetPeerAddr()
{
struct sockaddr_in6 addr_in;
    socklen_t len = sizeof(struct sockaddr_in6);
    ::getpeername(mSock,(struct sockaddr*)&addr_in, &len);
    InetAddress::ptr localaddr = std::make_shared<InetAddress>();
    if (addr_in.sin6_family == AF_INET) {
            char ip[16] = {0,};
            struct sockaddr_in *saddr = (struct sockaddr_in *)&addr_in;
            ::inet_ntop(AF_INET, &(saddr->sin_addr.s_addr), ip, sizeof(ip));
            localaddr->SetAddr(ip);
            localaddr->SetPort(ntohs(saddr->sin_port));
        } else if (addr_in.sin6_family == AF_INET6) {
            char ip[INET6_ADDRSTRLEN] = {0,};
            ::inet_ntop(AF_INET6, &(addr_in.sin6_addr), ip, sizeof(ip));
            localaddr->SetAddr(ip);
            localaddr->SetPort(ntohs(addr_in.sin6_port));
            localaddr->SetIsIpv6(true);
        }
    return localaddr;
}

void SocketOpt::SetTcpNoDelay(bool on)
{
    int optValue = on?1:0;
    ::setsockopt(mSock, IPPROTO_TCP, TCP_NODELAY, &optValue, sizeof(optValue));
}

void SocketOpt::SetReuseAddr(bool on)
{
int optValue = on?1:0;
    ::setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof(optValue));
}

void SocketOpt::SetReusePort(bool on)
{
    int optValue = on?1:0;
    ::setsockopt(mSock, SOL_SOCKET, SO_REUSEPORT, &optValue, sizeof(optValue));
}

void SocketOpt::setKeepAlive(bool on)
{
    int optValue = on?1:0;
    ::setsockopt(mSock, SOL_SOCKET, SO_KEEPALIVE, &optValue, sizeof(optValue));
}

void SocketOpt::SetNonBlocking(bool on)
{
    int flag = ::fcntl(mSock, F_GETFL, 0);
    if(on)
    {
        flag |= O_NONBLOCK;
    }
    else
    {
        flag &= ~O_NONBLOCK;
    }
    ::fcntl(mSock, F_SETFL, flag);
}