#include "pch.h"
#include "EventLock.h"

EventLock::EventLock()
{
    _hEvent = CreateEvent(nullptr, FALSE, TRUE, nullptr);
    if (_hEvent == nullptr) {
        std::cerr << "CreateEvent failed: " << GetLastError() << "\n";
        std::terminate();
    }
}

EventLock::~EventLock()
{
    CloseHandle(_hEvent);
}

void EventLock::ProcessLock()
{
    DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
    if (dwWait != WAIT_OBJECT_0) {
        std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
        std::terminate();
    }
}

void EventLock::ProcessUnLock()
{
    if (SetEvent(_hEvent) == false) {
        std::cerr << "SetEvent failed: " << GetLastError() << "\n";
        std::terminate();
    }
}
