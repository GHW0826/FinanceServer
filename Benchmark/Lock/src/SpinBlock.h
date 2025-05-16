#pragma once
#include "ILockTest.h"
#include <atomic>
using namespace std;

class SpinLock : public ILockTest
{
public:
    void lock_shared() override
    {
        bool expected = false;
        while (!_flag.compare_exchange_strong(expected, true)) {
            expected = false;
        }
    }
    void unlock_shared() override
    {
        _flag.store(false);
    }
    void lock_exclusive() override
    {
        bool expected = false;
        while (!_flag.compare_exchange_strong(expected, true)) {
            expected = false;
        }
    }
    void unlock_exclusive() override
    {
        _flag.store(false);
    }
private:
    std::atomic<bool> _flag = false;
};
