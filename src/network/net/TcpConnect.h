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
#include <list>
#include <sys/uio.h>

namespace Live
{
    namespace network 
    {
        struct BufferNode
        {
            BufferNode(void *buff, size_t s)
            :addr(buff),size(s)
            {

            }
            void *addr{nullptr};
            int size{0};
        };
        class TcpConnect;
        using TcpConnectionPtr = std::shared_ptr<TcpConnect>;
        using CloseConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
        using MessageCallback = std::function<void(const TcpConnectionPtr&, MsgBuffer &buffer)>;
        using WriteCompleteCallback =  std::function<void(const TcpConnectionPtr&)>;
        using BufferNodePtr = std::shared_ptr<BufferNode>;
        class TcpConnect : public Connection
        {
        public:
            TcpConnect(EventLoop*loop, int socket_fd, const InetAddress &localAddr, const InetAddress &peerAddr);
            ~TcpConnect();

            void SetCloseCallback(const CloseConnectionCallback &cb);
            void SetCloseCallback(CloseConnectionCallback &&cb);

            void SetRecvMsgCallback(const MessageCallback& cb);
            void SetRecvMsgCallback(MessageCallback&& cb);

            void SetWriteCompleteCallback(const WriteCompleteCallback &cb);
            void SetWriteCompleteCallback(const WriteCompleteCallback &&cb);
            void Send(std::list<BufferNodePtr> &list);
            void Send(const char* buffer, int size);

            void OnClose() override;
            void OnRead() override;
            void OnWrite() override;
            void ForceClose() override;
            void OnError(const std::string &msg);
        private:
            void SendInLoop(std::list<BufferNodePtr> &list);
            void SendInLoop(const char* buffer, int size);
            bool mClose{false};
            CloseConnectionCallback mCloseCb;
            MsgBuffer mMessageBuffer;
            MessageCallback mMessageCallback;
            WriteCompleteCallback mWriteComCallback;
            std::vector<struct iovec> mIovecList;

        };

    }
}