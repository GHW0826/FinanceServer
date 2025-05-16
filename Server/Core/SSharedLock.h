#pragma once
#include "IRWLock.h"

class SSharedLock : public IRWLock
{
public:
    SSharedLock();
public:
    void ProcessReadLock() override;
    void ProcessReadUnLock() override;
    void ProcessWriteLock() override;
    void ProcessWriteUnLock() override;
private:
    SRWLOCK _srwlock;
};

