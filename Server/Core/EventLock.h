#pragma once
#include <Windows.h>
#include <iostream>
#include "ILock.h"

class EventLock : public ILock
{
public:
	EventLock();
	~EventLock();
public:
	void ProcessLock() override;
	void ProcessUnLock() override;
private:
	HANDLE _hEvent;
};

