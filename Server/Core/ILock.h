#pragma once

class ILock abstract
{
public:
	void lock() 
	{
		ProcessLock();
	}
	void unlock() 
	{
		ProcessUnLock();
	}
protected:
	virtual void ProcessLock() = 0;
	virtual void ProcessUnLock() = 0;
};

