#include "pch.h"
#include "StdSharedLock.h"

void StdSharedLock::ProcessReadLock()
{
    _sm.lock_shared();
}

void StdSharedLock::ProcessReadUnLock()
{
    _sm.unlock_shared();
}

void StdSharedLock::ProcessWriteLock()
{
    _sm.lock();
}

void StdSharedLock::ProcessWriteUnLock()
{
    _sm.unlock();
}
