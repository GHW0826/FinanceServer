#pragma once
#include "ILockTest.h"
#include <atomic>
#include <Windows.h>
#include <iostream>
#include <thread>
using namespace std;

class SELock : public ILockTest
{
	enum : int {
		MAX_SPIN_COUNT = 1'000,
	};
public:
	SELock(int spinCount = MAX_SPIN_COUNT)
	{
		_hEvent = CreateEvent(nullptr, FALSE, TRUE, nullptr);
		_spinCount = spinCount;
		if (_hEvent == nullptr) {
			std::cerr << "CreateEvent failed: " << GetLastError() << "\n";
			std::abort();
		}
	}
	~SELock() {
		CloseHandle(_hEvent);
	}
public:
	void lock_shared() override
	{
		bool expected = false;
		while (true) {
			for (int spinCount = 0; spinCount < _spinCount; ++spinCount) {
				if (_flag.compare_exchange_strong(expected, true)) {
					return;
				}
			}
			DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
			if (dwWait != WAIT_OBJECT_0) {
				std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
				std::abort();
			}
		}
	}
	void unlock_shared() override
	{
		_flag.store(false);
		if (SetEvent(_hEvent) == false) {
			std::cerr << "SetEvent failed: " << GetLastError() << "\n";
			std::abort();
		}
	}
	void lock_exclusive() override
	{
		while (true) {
			for (int spinCount = 0; spinCount < _spinCount; ++spinCount) {
				bool expected = false;
				if (_flag.compare_exchange_strong(expected, true)) {
					return;
				}
			}

			DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
			if (dwWait != WAIT_OBJECT_0) {
				std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
				std::abort();
			}
		}
	}
	void unlock_exclusive() override
	{
		_flag.store(false);
		if (SetEvent(_hEvent) == false) {
			std::cerr << "SetEvent failed: " << GetLastError() << "\n";
			std::abort();
		}
	}
private:
	int _spinCount = MAX_SPIN_COUNT;
	std::atomic<bool> _flag = false;
	HANDLE _hEvent;
};
