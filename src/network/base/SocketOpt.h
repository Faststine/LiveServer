#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <memory>
#include "InetAddress.h"

namespace Live
{
    namespace network 
    {
        class SocketOpt
        {
        public:
            SocketOpt(int sock, bool ipV6 = false);
            ~SocketOpt() = default;
            static int CreateNonblockingTcpSocket(int family);
            static int CreateNonblockingUdpSocket(int family);

            void BindAddress(const InetAddress &localaddr);
            int Listen();
            int Accept(InetAddress *peeraddr);

            int Connect(const InetAddress &addr);
            InetAddress::ptr GetLocalAddr();
            InetAddress::ptr GetPeerAddr();

            void SetTcpNoDelay(bool on);
            void SetReuseAddr(bool on);
            void SetReusePort(bool on);
            void setKeepAlive(bool on);
            void SetNonBlocking(bool on);
        private:
            int mSock{-1};
            bool mIsIpv6;
        };

    }
}