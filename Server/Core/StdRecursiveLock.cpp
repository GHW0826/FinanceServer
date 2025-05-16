#include "pch.h"
#include "StdRecursiveLock.h"

void StdRecursiveLock::ProcessLock()
{
    _m.lock();
}

void StdRecursiveLock::ProcessUnLock()
{
    _m.unlock();
}
