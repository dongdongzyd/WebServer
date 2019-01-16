//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月14日22:20:19
//封装条件变量
#pragma once
#include "noncopyable.h"
#include "MutexLock.h"
#include <pthread.h>
#include <time.h>
#include <errno.h>

class Condition: noncopyable
{
public:
    explicit Condition(MutexLock& mutex): mutex_(mutex)
    {
        pthread_cond_init(&cond_, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond_);
    }
    void wait()
    {
        pthread_cond_wait(&cond_, mutex_.get());
    }
    void notify()
    {
        pthread_cond_signal(&cond_);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&cond_);
    }
    bool waitForSeconds(int seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);    //获得从1970.1.1 开始的时间 放在 abstime结构体里。
        abstime.tv_sec += static_cast<time_t>(seconds);
        
        return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.get(), &abstime);

    }
    
private:
    MutexLock& mutex_;
    pthread_cond_t cond_;
};