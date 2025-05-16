#pragma once
#include "ILockTest.h"
#include <mutex>
using namespace std;

class StdRecursiveLock : public ILockTest
{
public:
    void lock_shared() override
    {
        _m.lock();
    }
    void unlock_shared() override
    {
        _m.unlock();
    }
    void lock_exclusive() override
    {
        _m.lock();
    }
    void unlock_exclusive() override
    {
        _m.unlock();
    }
private:
    std::recursive_mutex _m;
};
