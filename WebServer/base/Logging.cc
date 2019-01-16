//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月15日21:32:19
#include "Logging.h"
#include "CurrentThread.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include <assert.h>
#include <iostream>
#include <time.h>    
#include <sys/time.h>   //for gettimeofday()

static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static AsyncLogging* AsyncLogger;

std::string Logger::logFileName_ = "/xcw_WebServer.log";

void once_init()
{
    AsyncLogger = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger->start();
}

void output(const char* msg, int len)
{
    pthread_once(&once_control, once_init);
    AsyncLogger->append(msg, len);
}

Logger::Logger(const char* filename, int line)
  : impl_(filename, line)
{ }

Logger::~Logger()
{
    impl_.stream_ << " -- " << impl_.basename_ << ' ' << impl_.line_ << '\n';
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
} 

Logger::Impl::Impl(const char* filename, int line)
   : stream_(),
     line_(line),
     basename_(filename)
{
    formatTime();
}

void Logger::Impl::formatTime()
{
    time_t now;
    char str_t[26] = {0};
    now = time(NULL);    //since 1970's sec
    struct tm* p_time = localtime(&now);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}