#pragma once
#include "ILockTest.h"
#include <Windows.h>

class CriticalSectionLock : public ILockTest
{
    enum : int {
        MAX_SPIN_COUNT = 2'000,
    };
public:
    CriticalSectionLock(int spinCount = MAX_SPIN_COUNT)
    {
		_spinCount = spinCount;
        if (InitializeCriticalSectionAndSpinCount(&_cs, _spinCount) == false) {
            std::terminate();
        }
    }
    ~CriticalSectionLock()
    {
        DeleteCriticalSection(&_cs);
    }
public:
    void lock_shared() override
    {
        EnterCriticalSection(&_cs);
    }
    void unlock_shared() override
    {
        LeaveCriticalSection(&_cs);
    }
    void lock_exclusive() override
    {
        EnterCriticalSection(&_cs);

    }
    void unlock_exclusive() override
    {
        LeaveCriticalSection(&_cs);
    }
private:
    int _spinCount;
    CRITICAL_SECTION  _cs;
};