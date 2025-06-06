#pragma once
#include "network/base/InetAddress.h"
#include "network/base/MsgBuffer.h"
#include "network/net/Event.h"
#include "network/net/EventLoop.h"
#include "network/net/Connection.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>

namespace Live
{
    namespace network 
    {
        class TcpConnect;
        using TcpConnectionPtr = std::shared_ptr<TcpConnect>;
        using CloseConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
        using MessageCallback = std::function<void(const TcpConnectionPtr&, MsgBuffer &buffer)>;
        class TcpConnect : public Connection
        {
        public:
            TcpConnect(EventLoop*loop, int socket_fd, const InetAddress &localAddr, const InetAddress &peerAddr);
            ~TcpConnect();

            void SetCloseCallback(const CloseConnectionCallback &cb);
            void SetCloseCallback(CloseConnectionCallback &&cb);

            void SetRecvMsgCallback(const MessageCallback& cb);
            void SetRecvMsgCallback(MessageCallback&& cb);
            void OnClose() override;
            void OnRead() override;
            void ForceClose() override;
            void OnError(const std::string &msg);
        private:
            bool mClose{false};
            CloseConnectionCallback mCloseCb;
            MsgBuffer mMessageBuffer;
            MessageCallback mMessageCallback;
        };

    }
}