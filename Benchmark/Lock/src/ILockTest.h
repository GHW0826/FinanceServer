#pragma once

class ILockTest abstract
{
public:
    virtual void lock_shared() = 0;
    virtual void unlock_shared() = 0;
    virtual void lock_exclusive() = 0;
    virtual void unlock_exclusive() = 0;
};
