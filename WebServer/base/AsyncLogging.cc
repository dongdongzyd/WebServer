//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月15日17:04:21
#include "AsyncLogging.h"
#include "LogFile.h"
#include <stdio.h>
#include <assert.h>
#include <functional>     //for std::function std::bind

AsyncLogging::AsyncLogging(std::string& logFileName, int flushInterval)
  : flushInterval_(flushInterval),
    running_(false),
    basename_(logFileName),
    thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
    mutex_(),
    cond_(mutex_),
    //准备了2块缓冲区
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_(),
    latch_(1)
{
    assert(logFileName.size() > 1);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

//前端生产者线程(多个)append到内存中
void AsyncLogging::append(const char* logline, int len)
{
    MutexLockGuard lock(mutex_);
    if(currentBuffer_->avail() > len)
        currentBuffer_->append(logline, len);
    else
    {
        //使用移动语义，currentBuffer_不再有指向了
        buffers_.push_back(std::move(currentBuffer_));

        if(nextBuffer_)
        {
            currentBuffer_ = std::move(nextBuffer_);
        }
        else
        {
            //这种情况极少发生，前端写入线程太快，把2块都写完了
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_.append(logline, len);
        //保证了不是一有数据就开始写入文件，效率较高
        cond_.notify();
    }
}

//后端消费者线程(一个)，负责把缓冲区的数据写入到文件
void AsyncLogging::threadFunc()
{
    assert(running_ == true);
    latch_.countDown();
    LogFile output(basename_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2-》length() == 0);
        assert(buffersToWrite.empty());

        {
            MutexLockGuard lock(mutex_);
            //这里没用while是因为不必非要empty为空，只要cond_超时时间到就行
            if(buffers_.empty())   
            {
                cond_.waitForSeconds(flushInterval_);
            }
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);
            //这一步交换的目的在于后面的代码在写日志到文件时,可以在临界区之外
            //安全的访问buffersToWrite,大大减少了临界区，效率高。
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_)
            {
                //目的是确保前端线程始终都有一个预备的buffer可以调配，
                //减少前端临界区重新分配内存的概率，缩短前端临界区长度。
                nextBuffer_ = std::move(newBuffer2);
            }
        }
        
        assert(!buffersToWrite.empty());

        if(buffersToWrite.size() > 25)
        {
            //出现日志堆积现象，就只留前2块， 很少发生
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }

        for(size_t i = 0; i < buffersToWrite.size(), i++)
        {
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }

        if(buffersToWrite.size() > 2)
           //仅保留两个buffer，用于newBuffer1与newBuffer2
            buffersToWrite.resize(2);
        
        if(!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if(!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}