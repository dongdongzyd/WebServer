//Author: xcw
//Email:  xcw_coder@qq.com
//2019年01月14日22:33:42
//CountDownLatch的主要作用是确保Thread中传进去的线程函数
//func真的启动以后，外层的start才返回
#pragma once;
#include "MutexLock.h"
#include "Condition.h"
#include "noncopyable.h"

class CountDownLatch: noncopyable
{
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};
