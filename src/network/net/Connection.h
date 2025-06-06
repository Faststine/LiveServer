#pragma once
#include "network/base/InetAddress.h"
#include "network/net/Event.h"
#include "network/net/EventLoop.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>

namespace Live
{
    namespace network 
    {
        enum
        {
            kNormalContext = 0,
            kRtmpContext,       // rtmp
            kHttpContext,       // http
            kUserContext,
            kFlvContext         // flv
        };
        using ContextPtr = std::shared_ptr<void>;
        class Connection;
        using ConnectionPtr = std::shared_ptr<Connection>;
        using ActiveCallback = std::function<void(const ConnectionPtr&)>;
        class Connection : public Event
        {
        public:
            Connection(EventLoop*loop, int fd, const InetAddress &localAddr, const InetAddress &peerAddr);
            virtual ~Connection() = default;
            virtual void ForceClose() = 0;

            void SetLocalAddr(const InetAddress &local);
            void SetPeerAddr(const InetAddress &peer);
            const InetAddress &LocalAddr() const;
            const InetAddress &PeerAddr() const;

            void SetContext(int type, const std::shared_ptr<void> &context);
            void SetContext(int type, std::shared_ptr<void> &&context);
            template <typename T> 
            std::shared_ptr<T> GetContext(int type) const
            {
                auto iter = mContext.find(type);
                if (iter != mContext.end())
                {
                    return std::dynamic_pointer_cast<T>(iter->second);
                }
                return std::shared_ptr<T>();
                
            }
            void ClearContxt(int type);
            void ClearContxt();

            void SetActiveCallback(const ActiveCallback &callback);
            void SetActiveCallback(const ActiveCallback &&callback);
            void Active();
            void Deactive();
        protected:
            InetAddress mLocalAddr;
            InetAddress mPeerAddr;
        private:
        std::unordered_map<int, ContextPtr> mContext; 
        ActiveCallback mActiveCb;
 
        std::atomic<bool> active_;      // 设置激活状态,不同线程调用
        };

    }
}