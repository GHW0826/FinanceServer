#pragma once
#include <type_traits>
#include "ILockTest.h"

template<typename T>
class ReadLockGuard
{
    static_assert(std::is_base_of_v<ILockTest, T>, "T must inherit from ILockTest");
public:
    ReadLockGuard(T& lock)
        : _lock(lock)
    {
        _lock.lock_shared();
    }
    ~ReadLockGuard()
    {
        _lock.unlock_shared();
    }
private:
    T& _lock;
};


template<typename T>
class WriteLockGuard
{
    static_assert(std::is_base_of_v<ILockTest, T>, "T must inherit from ILockTest");
public:
    WriteLockGuard(T& lock)
        : _lock(lock)
    {
        _lock.lock_exclusive();
    }
    ~WriteLockGuard()
    {
        _lock.unlock_exclusive();
    }
private:
    T& _lock;
};
