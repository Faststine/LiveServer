#pragma once
#include "Event.h"

namespace tmms 
{
    namespace network 
    {
        class PipEvent : public Event
        {
        private:
            int write_fd_{-1};
        public:
            PipEvent(EventLoop *loop);
            ~PipEvent();
            using ptr = std::shared_ptr<PipEvent>;
            void OnRead() override;
            void OnClose() override;
            void OnError(const std::string &msg) override;
            void Write(const char *data, size_t len);
        };
    }
}