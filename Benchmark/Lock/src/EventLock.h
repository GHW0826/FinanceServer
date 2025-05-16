#pragma once
#include <Windows.h>
#include <iostream>
#include "ILockTest.h"
using namespace std;

class EventLock : public ILockTest
{
public:
    EventLock() {
        _hEvent = CreateEvent(nullptr, FALSE, TRUE, nullptr);
        if (_hEvent == nullptr) {
            std::cerr << "CreateEvent failed: " << GetLastError() << "\n";
            std::abort();
        }
    }
    ~EventLock() {
        CloseHandle(_hEvent);
    }
public:
    void lock_shared() override {
        DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
            std::abort();
        }
    }

    void unlock_shared() override {
        if (!SetEvent(_hEvent)) {
            std::cerr << "SetEvent failed: " << GetLastError() << "\n";
            std::abort();
        }
    }
    void lock_exclusive() override {
        DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
            std::abort();
        }
    }
    void unlock_exclusive() override {
        if (!SetEvent(_hEvent)) {
            std::cerr << "SetEvent failed: " << GetLastError() << "\n";
            std::abort();
        }
    }
private:
    HANDLE _hEvent;
};

