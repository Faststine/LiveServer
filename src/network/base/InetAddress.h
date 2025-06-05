#pragma once
#include <sys/socket.h> // socket
#include <arpa/inet.h>  // 地址转换
#include <netinet/in.h>
#include <bits/socket.h> // socket数据类型
#include <string>
#include <memory>

namespace Live
{
    namespace network 
    {
        class InetAddress
        {

        public:
            using ptr = std::shared_ptr<InetAddress>;
            InetAddress(const std::string &ip, uint16_t port, bool bv6=false);
            InetAddress(const std::string &host, bool bv6=false);
            InetAddress() = default;
            ~InetAddress();

            void SetHost(const std::string &host);
            void SetAddr(const std::string &addr);
            void SetPort(const uint16_t port);
            void SetIsIpv6(const bool isIpv6);

            const std::string &IP() const;
            uint32_t IPv4() const;
            std::string ToIpPort() const;
            uint16_t Port() const;
            void GetSocketAddr(struct sockaddr *saddr) const;

            bool IsIpv6() const;
            bool IsWanIp() const;
            bool IsLanIp() const;
            bool IsLoopbackIp() const;

            void GetIpAndPort(const std::string &host, std::string &ip, std::string &port);
        private:
            uint32_t IPv4(const char* ip) const;
            std::string addr_;
            std::string port_;
            bool is_ipv6_{false}; // 默认不是ipv6
        };

    }
}