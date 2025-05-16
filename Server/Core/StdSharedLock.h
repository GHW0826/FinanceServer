#pragma once
#if __cplusplus >= 201703L
#include "IRWLock.h"
#include <shared_mutex>

class StdSharedLock : public IRWLock
{
public:
	void ProcessReadLock() override;
	void ProcessReadUnLock() override;
	void ProcessWriteLock() override;
	void ProcessWriteUnLock() override;
public:
	std::shared_mutex _sm;
};

#endif