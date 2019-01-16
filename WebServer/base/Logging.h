//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月15日21:19:46
#pragma once
#include "LogStream.h"
#include <pthread.h>
#include <string>
#include <stdio.h>

class AsyncLogging;

class Logger
{
public: 
    Logger(const char* filename, int line);
    ~Logger();
    LogStream& stream() { return impl_.stream_; }
    
    static void setLogFileName(std::string filename)
    {
        logFileName_ = filename;
    }
    static std::string getLogFileName()
    {
        return logFileName_;
    }

private: 
    class Impl 
    {
    public: 
        Impl(const char* filename, int line);
        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };
    Impl impl_;
    static std::string logFileName_;
};

//对外接口 LOG
#define LOG Logger(__FILE, __LINE__).stream()