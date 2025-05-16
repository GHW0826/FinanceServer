#include "pch.h"
#include "StdLock.h"

void StdLock::ProcessLock()
{
    _m.lock();
}

void StdLock::ProcessUnLock()
{
    _m.unlock();
}
