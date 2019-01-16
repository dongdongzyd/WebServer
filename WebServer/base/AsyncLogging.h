//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月15日16:48:05
#pragma once
#include "CountDownLatch.h"
#include "MutexLock.h"
#include "Thread.h"
#include "LogStream.h"
#include "noncopyable.h"
#include <string>
#include <vector>
#include <memory>

class AsyncLogging : noncopyable
{
public: 
    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging()
    {
        if(running_)
            stop();
        
    }
    //前端生产者线程调用(将日志写到当前缓冲区)，可以有多个
    void append(const char* logline, int len);

    void start()
    {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop()   //没分析线程安全
    {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private: 
    //后端消费者线程调用(将数据写到日志文件中区)
    void threadFunc();   //线程函数里维护着一个LogFile文件
    typedef FixedBuffer<kLargerBuffer> Buffer;
    typedef std::vector<std::unique<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;
    
    //超时时间内当前缓冲区没有写满，仍然将缓冲区的数据写到文件
    const int flushInterval_;
    bool running_;
    std::string basename_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;    //当前缓冲区
    BufferPtr nextBuffer_;       //预备缓冲区
    BufferVector buffers_;       
    CountDownLatch latch_;
};