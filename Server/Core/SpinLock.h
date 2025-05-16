#pragma once
#include "ILock.h"
#include <atomic>

class SpinLock : public ILock
{
public:
	void ProcessLock() override;
	void ProcessUnLock() override;
private:
	std::atomic<bool> _flag = false;
};

