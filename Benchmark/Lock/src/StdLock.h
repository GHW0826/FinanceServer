#pragma once
#include "ILockTest.h"
#include <Windows.h>
#include <mutex>
#include <iostream>
using namespace std;

class StdLock : public ILockTest
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
    std::mutex _m;
};
