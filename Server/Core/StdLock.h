#pragma once
#include "ILock.h"
#include <mutex>

class StdLock : public ILock
{
public:
	void ProcessLock() override;
	void ProcessUnLock() override;
public:
	std::mutex _m;
};

