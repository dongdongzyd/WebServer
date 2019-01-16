//Author: xcw
//Email: xcw_coder@qq.com
//2019年01月15日16:21:54
#include "LogStream.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>  //for std::reverse 

const char digits[] = "9876543210123456789";
const char* zero = digits + 6;

//From Muduo
template<typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;

    do
    {
        // last digit
        int lsd = static_cast<int>(i % 10); 
        i /= 10;
        *p++ = zero[lsd];
    } while(i != 0);
    
    if(value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);   //顺序倒置

    return p - buf;
}

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargerBuffer>;

//将整数转化成字符串加入到buffer_中
template<typename T>
void LogStream::formatInteger(T v)
{
    //buffer_容不下kMaxNumericSize个字符串会被直接丢弃
    if(buffer_.avail() >= kMaxNumericSize)
    {
        size_t len = convert(buffer_.current(), v);
        buffer_.add(len);
    }
}

LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v)
{
    formatInteger(v); 
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if(buffer_.avail() >= kMaxNumericSize)
    {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double v)
{
    if(buffer_.avail() >= kMaxNumericSize)
    {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}