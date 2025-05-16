#pragma once
#include "ILockTest.h"
#include <atomic>
#include <Windows.h>
#include <thread>
using namespace std;

// spin and yield lock
class SYLock : public ILockTest
{
	enum :int {
		MAX_SPIN_COUNT = 1'000,
	};
public:
	SYLock(int spinCount = MAX_SPIN_COUNT)
	{
		_spinCount = spinCount;
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
			this_thread::yield();
		}
	}
	void unlock_shared() override
	{
		_flag.store(false);
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
			this_thread::yield();
		}
	}
	void unlock_exclusive() override
	{
		_flag.store(false);
	}
private:
	int _spinCount;
	std::atomic<bool> _flag = false;
};
