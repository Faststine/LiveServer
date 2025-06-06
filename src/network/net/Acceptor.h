#pragma once
#include "network/base/InetAddress.h"
#include "network/base/SocketOpt.h"
#include "network/net/Event.h"
#include "network/net/EventLoop.h"
#include <functional>
namespace Live
{
    namespace network 
    {
        using acceptCallback = std::function<void(int sock, const InetAddress &addr)>;
        class Acceptor : public Event
        {
        public:
            Acceptor(EventLoop *loop, const InetAddress &addr);
            ~Acceptor();

            void SetAcceptCallback(const acceptCallback &cb);
            void SetAcceptCallback(const acceptCallback &&cb);
            void Start();
            void Stop();
            void OnRead() override;
            void OnError(const std::string &msg) override;
            void OnClose() override;
        private:
            void Open();
            InetAddress mInetAddr;
            acceptCallback mAcceptCallback;
            SocketOpt *mSocketOpt{nullptr};
        };

    }
}