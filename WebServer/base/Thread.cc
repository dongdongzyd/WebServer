//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月14日23:18:19
#include "Thread.h"
#include "CurrentThread.h"
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/syscall.h>
#include <sys/prctl.h>

using namespace std;

//每个线程都会有这些变量，这些变量的值不会受其他线程影响，是线程局部变量
namespace CurrentThread
{
    __thread int t_cacheTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "Default";
}

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid()
{
    if(t_cacheTid == 0)
    {
        t_cacheTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d", t_cacheTid);
    }
}

//为了在线程中保留name, tid这些数据
struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    string name_;
    pid_t* tid_;
    CountDownLatch* latch_;

    ThreadData(const ThreadFunc& func, 
               const string& name, 
               pid_t* tid, 
               CountDownLatch* latch)
    : func_(func),
      name_(name),
      tid_(tid),
      latch_(latch)
    { }

    void runInThread()
    {
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = NULL;

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        //设置进程名字，第二个参数是进程名字符串，长度最多16字节
        ::prctl(PR_SET_NAME, CurrentThread::t_threadName);

        func_();
        CurrentThread::t_threadName = "finished";
    }

};

void* startThread(void* obj)
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    
    return NULL;
}

Thread::Thread(const ThreadFunc& func, const string& name)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(0),
      func_(func),
      name_(name),
      latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_)
        pthread_detach(pthreadId_);
}

void Thread::setDefaultName()
{
    if(name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
    if(pthread_create(&phtreadId_, NULL, &startThread, data))
    {
        started_ = false;
        delete data;
    }
    else
    {
        latch_.wait();   //确保得到tid_的数值之后在返回start()函数
        assert(tid > 0);
    }
}

int Thread::join()
{
    assert(started_);
    aseert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}