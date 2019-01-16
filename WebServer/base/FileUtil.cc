//Author: xcw
//Email:  xcw_coder@qq.com 
//2019年01月15日00:32:43
#include "FileUtil.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

AppendFile::AppendFile(string filename)     
: fp_(fopen(filename.c_str(), "ae"))  //  'e' for close_on_exec
{
    assert(fp);
    //设定文件指针的缓冲区为buffer_，如果不指定，文件指针缓冲区默认
    //文件缓冲区超过64kb会自动flush，不需要手动flush
    ::setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFile::~AppendFile()
{
    fclose(fp_);
}

void AppendFile::append(const char* logline, size_t len)
{
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    while(remain > 0)
    {
        size_t x = this->write(logline+n, remain);
        if(x == 0)
        {
            int err = ferror(fp_);  //出错返回非0值
            if(err)
                fprintf(stderr, "AppendFile::append() failed !\n");
            break;
        }
        n += x;
        remain = len - n;
    }
}

size_t AppendFile::write(const char* logline, size_t len)
{
    return ::fwrite_unlocked(logline, 1, len, fp_);  //thread safe
}

void AppendFile::flush()
{
    ::fflush(fp_);
}