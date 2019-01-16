//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月15日15:25:27
//封装了一块缓冲区，用于前端线程将日志文件写入到该缓冲区，
//同时格式化输出，重载<<运算符
#pragma once
#include "noncopyable.h"
#include <assert.h>
#include <string>
#include <string.h>

class AsyncLogging;
const int kSmallBuffer = 4000;            //4KB
const int kLargerBuffer = 4000 * 1000;    //4MB

template<int SIZE>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer() : cur_(data_)
    { }

    ~FixedBuffer()
    { }

    void append(const char* buf, size_t len)
    {
        if(avail() > static_cast<int>(len))
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }
    
    const char* data() const { return data_; }
    int length() const { return static_cast<int>(cur_ - data_); }

    char* current() { return cur_; }
    int avail() const { return static_cast<int>(end() - cur_); }
    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }
    void bzero() { memset(data_, 0, sizeof data_); }

private:
    const char* end() const {return data_ + sizeof data_;}  //类似迭代器end()

    char data_[SIZE];
    char* cur_;
};

class LogStream : noncopyable
{
public: 
    typedef FixedBuffer<kSmallBuffer> Buffer;

    LogStream& operator<<(bool v)
    {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }
    //重载<<运算符
    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    LogStream& operator<<(const void*);

    LogStream& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream& operator<<(double);
    LogStream& operator<<(long double);

    LogStream& operator<<(char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }

    LogStream& operator<<(const char* str)
    {
        if(str)
            buffer_.append(str, strlen(str));
        else
            buffer_.append("(null)", 6);
        return *this;
    }

    LogStream& operator<<(const unsigned char* str)
    {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    LogStream& operator<<(const std::string& v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }

    void append(const char* data, int len) { buffer_.append(data, len); }
    const Buffer& buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    template<typename T>
    void formatInteger(T);

    Buffer buffer_;

    static const int kMaxNumericSize = 32;
};