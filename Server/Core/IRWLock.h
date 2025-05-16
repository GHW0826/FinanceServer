#pragma once

class IRWLock abstract
{
public:
	void lock_shared() 
	{
		ProcessReadLock();
	}
	void unlock_shared() 
	{
		ProcessReadUnLock();
	}
	void lock_exclusive() 
	{
		ProcessWriteLock();
	}
	void unlock_exclusive() 
	{
		ProcessWriteUnLock();
	}
protected:
	virtual void ProcessReadLock() = 0;
	virtual void ProcessReadUnLock() = 0;
	virtual void ProcessWriteLock() = 0;
	virtual void ProcessWriteUnLock() = 0;
};

