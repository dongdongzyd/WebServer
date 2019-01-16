//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月14日22:39:51
#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count) : mutex_(), condition_(mutex_), count_(count)
{ }

void CountDownLatch::wait()
{
    MutexLockGuard lock(mutex_);
    while(count_ > 0)
        condition_.wait();
}

void CountDownLatch::countDown()
{
    MutexLockGuard lock(mutex_);
    --count_;
    if(count_ == 0)
        condition_.notifyAll();
}