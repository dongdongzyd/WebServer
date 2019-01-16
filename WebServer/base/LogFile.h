//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月15日01:17:08
//进一步封装FileUtil类
#pragma once
#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"
#include <string>
#include <memory>

class LogFile : noncopyable
{
public:
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool roolFile();

private:
    void append_unlocked(const char* logline, int len);

    const std::string basename_;
    //计数器，每flushEveryN次就flush
    const int flushEveryN_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};