#pragma once
#include "ILock.h"

class StdRecursiveLock : public ILock
{
public:
	void ProcessLock() override;
	void ProcessUnLock() override;
private:
	std::recursive_mutex _m;
};

