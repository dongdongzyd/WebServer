// Author: xcw
// Email:  xcw_coder@qq.com
//2019年01月14日21:53:46
#pragma once

class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};