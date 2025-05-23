#pragma once
#include "Types.h"


//[wwwwwwww][wwwwwwww][rrrrrrrr][rrrrrrrr]
// W : WriteFlag (Exclusive Lock Owner ThreadId)
// R : ReadFlag (Shared Lock Count)

// W -> R : (O)
// R -> W : (X)
class Lock
{
	enum :uint32 {
		AQUIRE_TIMEOUT_TICK	= 10'000,
		MAX_SPIN_COUNT		= 5'000,
		WRITE_THREAD_MASK	= 0xFFFF'0000,
		READ_COUNT_MASK		= 0x0000'FFFF,
		EMPTY_FLAG			= 0x0000'0000
	};
public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);
private:
	Atomic<uint32> _lockFlag;
	uint16 _writeCount = 0;
};


class SReadLockGuard
{
public:
	SReadLockGuard(Lock& lock, const char* name) 
		: _lock(lock), _name(name) 
	{ 
		_lock.ReadLock(name);
	}
	~SReadLockGuard() 
	{ 
		_lock.ReadUnlock(_name); 
	}
private:
	Lock& _lock;
	const char* _name;
};


class SWriteLockGuard
{
public:
	SWriteLockGuard(Lock& lock, const char* name) 
		: _lock(lock), _name(name)
	{ 
		_lock.WriteLock(name); 
	}
	~SWriteLockGuard() 
	{
		_lock.WriteUnlock(_name); 
	}
private:
	Lock& _lock;
	const char* _name;
};