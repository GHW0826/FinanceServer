#include "pch.h"
#include "SpinLock.h"

void SpinLock::ProcessLock()
{
    bool expected = false;
    while (!_flag.compare_exchange_strong(expected, true)) {
        expected = false;
    }
}

void SpinLock::ProcessUnLock()
{
    _flag.store(false);
}
