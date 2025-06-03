/*
 * @Author: heart1128 1020273485@qq.com
 * @Date: 2024-06-02 17:00:58
 * @LastEditors: heart1128 1020273485@qq.com
 * @LastEditTime: 2024-06-03 11:36:23
 * @FilePath: /Live/src/base/TaskMgr.h
 * @Description:  learn 
 */
#pragma once
#include "Task.h"
#include "NonCopyable.h"
#include "Singleton.h"
#include <unordered_set>
#include <mutex>

namespace Live
{
    namespace base
    {
        class TaskMgr : public NonCopyable      // 基本管理类都是单例
        {
        public:
            TaskMgr() = default;
            ~TaskMgr() = default;
        
        public:
            void OnWork();
            bool Add(Task::ptr &task);
            bool Del(Task::ptr &task);

        private:
            std::unordered_set<Task::ptr> tasks_; // 直接遍历使用hash
            std::mutex lock_;
        };
        // 方便单例访问
        #define sTaskMgr Live::base::Singleton<Live::base::TaskMgr>::Instance()
    } 
} // namespace Live
