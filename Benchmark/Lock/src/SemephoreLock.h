#pragma once
#include <Windows.h>
#include <iostream>
#include "ILockTest.h"
using namespace std;

class SemephoreLock : public ILockTest
{
public:
    SemephoreLock() {
        _hsem = CreateSemaphore(nullptr, MAX_SEM_COUNT, MAX_SEM_COUNT, nullptr);
        if (_hsem == nullptr) {
            std::cerr << "CreateSemaphore failed: " << GetLastError() << "\n";
            std::terminate();
        }
    }
    ~SemephoreLock() {
        CloseHandle(_hsem);
    }
public:
    void lock_shared() override {
        DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
            std::terminate();
        }
    }

    void unlock_shared() override {
        if (!SetEvent(_hEvent)) {
            std::cerr << "SetEvent failed: " << GetLastError() << "\n";
            std::terminate();
        }
    }

    void lock_exclusive() override {
        DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
            std::terminate();
        }
    }

    void unlock_exclusive() override {
        if (!SetEvent(_hEvent)) {
            std::cerr << "SetEvent failed: " << GetLastError() << "\n";
            std::terminate();
        }
    }
private:
    HANDLE _hsem;
    const int MAX_SEM_COUNT = 3;
};

