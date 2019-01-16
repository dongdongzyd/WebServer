//Author: xcw
//Email:  xcw_coder@qq.com
#pragma once

namespace CurrentThread
{
    extern __thread int t_catchTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char* t_threadName;

    void cacheTid();
    inline int tid()
    {
        if(__builtin_expect(t_catchTid == 0, 0))
        {
            cacheTid();
        }
        return t_catchTid;
    }
    
    inline const char* tidString()   //for logging
    {
        return t_tidString;
    }

    inline int tidStringLength() //for logging
    {
        return t_tidStringLength;
    }

    inline const char* name()
    {
        return t_threadName;
    }
}   //CurrentThread
