#pragma once
#if __cplusplus >= 201703L
#include "ILockTest.h"
#include <shared_mutex>

class StdSharedLock : public ILockTest
{
public:
    void lock_shared() override
    {
        _sm.lock_shared();
    }
    void unlock_shared() override
    {
        _sm.unlock_shared();
    }
    void lock_exclusive() override
    {
        _sm.lock();
    }
    void unlock_exclusive() override
    {
        _sm.unlock();
    }
public:
    std::shared_mutex _sm;
};

#endif
