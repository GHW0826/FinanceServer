#pragma once
#include "ILockTest.h"
#include <Windows.h>

using namespace std;

class SSharedLock : public ILockTest
{
public:
    SSharedLock()
    {
        InitializeSRWLock(&_srwlock);
    }
public:
    void lock_shared() override
    {
        AcquireSRWLockShared(&_srwlock);
    }
    void unlock_shared() override
    {
        ReleaseSRWLockShared(&_srwlock);
    }
    void lock_exclusive() override
    {
        AcquireSRWLockExclusive(&_srwlock);
    }
    void unlock_exclusive() override
    {
        ReleaseSRWLockExclusive(&_srwlock);
    }
private:
    SRWLOCK _srwlock;
};
