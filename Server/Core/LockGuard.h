#pragma once
#include <type_traits>
#include "IRWLock.h"
#include "ILock.h"

template<typename T>
class WriteLockGuard
{
    static_assert(
        std::is_base_of_v<ILock, T> || std::is_base_of_v<IRWLock, T>,
        "T must inherit from ILock or IRWLock"
        );
public:
    WriteLockGuard(T& lock)
        : _lock(lock)
    {
        _lock.lock();
    }
    ~WriteLockGuard()
    {
        _lock.unlock();
    }
private:
    T& _lock;
};


template<typename T>
class ReadLockGuard
{
    static_assert(std::is_base_of_v<IRWLock, T>, "T must inherit from IRWLock");
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
