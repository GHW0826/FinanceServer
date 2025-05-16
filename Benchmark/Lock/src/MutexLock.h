#pragma once
#include <Windows.h>
#include <iostream>
#include "ILockTest.h"
using namespace std;

class MutexLock : public ILockTest
{
public:
    MutexLock() {
        _hmut = CreateMutex(nullptr, FALSE, nullptr);
        if (_hmut == nullptr) {
            std::cerr << "CreateMutex failed: " << GetLastError() << "\n";
            std::abort();
        }
    }
    ~MutexLock() {
        CloseHandle(_hmut);
    }
public:
    void lock_shared() override {
        DWORD dwWait = WaitForSingleObject(_hmut, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
            std::abort();
        }
    }
    void unlock_shared() override {
        if (ReleaseMutex(_hmut) == false) {
            std::cerr << "ReleaseMutex failed: " << GetLastError() << "\n";
            std::abort();
        }
    }
    void lock_exclusive() override {
        DWORD dwWait = WaitForSingleObject(_hmut, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
            std::abort();
        }
    }
    void unlock_exclusive() override {
        if (ReleaseMutex(_hmut) == false) {
            std::cerr << "ReleaseMutex failed: " << GetLastError() << "\n";
            std::abort();
        }
    }
private:
    HANDLE _hmut;
};

