//Author: xcw
//Email:  xcw_coder@qq.com 
//2019年01月14日23:58:46
//封装了最底层的文件类，RAII技法
#pragma once
#include "noncopyable.h"
#include <string>

class AppendFile: noncopyable
{
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();
    // append会向文件写数据
    void append(const char* logline, const size_t len);
    void flush();
    
private:
    size_t write(const char* log, size_t len);
    FILE* fp_;
    char buffer_[64*1024];
};