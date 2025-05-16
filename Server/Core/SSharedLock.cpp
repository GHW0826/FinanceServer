#include "pch.h"
#include "SSharedLock.h"

SSharedLock::SSharedLock()
{
    InitializeSRWLock(&_srwlock);
}

void SSharedLock::ProcessReadLock()
{
    AcquireSRWLockShared(&_srwlock);
}

void SSharedLock::ProcessReadUnLock()
{
    ReleaseSRWLockShared(&_srwlock);
}

void SSharedLock::ProcessWriteLock()
{
    AcquireSRWLockExclusive(&_srwlock);
}

void SSharedLock::ProcessWriteUnLock()
{
    ReleaseSRWLockExclusive(&_srwlock);
}
