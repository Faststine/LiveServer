/*
 * @Author: heart1128 1020273485@qq.com
 * @Date: 2024-06-01 18:25:10
 * @LastEditors: heart1128 1020273485@qq.com
 * @LastEditTime: 2024-06-03 14:04:16
 * @FilePath: /Live/src/base/LogStream.h
 * @Description:  learn 
 */
#pragma once
#include "Logger.h"
#include <sstream>

namespace Live
{
   namespace base
   {
        extern Logger *g_logger;
        class LogStream
        {
        public:
            LogStream(Logger* logger, const char* file, int line, LogLevel logLevel, const char* func = nullptr);
            ~LogStream();
        
            template<class T>
            LogStream& operator << (const T& value);
        
        private:
            std::ostringstream stream_;
            Logger *logger_{nullptr};
        };

        /// @brief 重载<<直接到ostringstream流中
        /// @tparam T 
        /// @param value 
        /// @return 
        template <class T>
        LogStream &LogStream::operator<<(const T &value)
        {
            stream_ << value;
            return *this;
        }

   } // namespace base

} // namespace Live


#define LOG_TRACE   \
    if(g_logger && Live::base::g_logger->GetLogLevel() <= KTrace)   \
        Live::base::LogStream(g_logger, __FILE__, __LINE__, Live::base::KTrace, __func__) 

#define LOG_DEBUG   \
    if(g_logger && Live::base::g_logger->GetLogLevel() <= KDebug)   \
        Live::base::LogStream(g_logger, __FILE__, __LINE__, Live::base::KDebug, __func__) 

#define LOG_WARN   \
        Live::base::LogStream(g_logger, __FILE__, __LINE__, Live::base::KWarn) 

#define LOG_INFO   \
    if(g_logger && Live::base::g_logger->GetLogLevel() <= KInfo)   \
        Live::base::LogStream(g_logger, __FILE__, __LINE__, Live::base::KTrace) 

#define LOG_ERROR  \
    if(g_logger && Live::base::g_logger->GetLogLevel() <= KError)   \
        Live::base::LogStream(g_logger, __FILE__, __LINE__, Live::base::KError, __func__) 

// define不能在文件的末尾，至少加一个空行