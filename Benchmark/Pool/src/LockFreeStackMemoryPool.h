#pragma once
#include <Windows.h>
#include <atomic>

using namespace std;

enum {
	SLIST_ALIGNMENT = 16
};


DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	// [MemoryHeader][Data]
	MemoryHeader(int size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int size)
	{
		new(header)MemoryHeader(size); 
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int allocSize;
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class LockFreeStackMemoryPool
{
public:
	LockFreeStackMemoryPool(int allocSize)
		: _allocSize(allocSize)
	{
		::InitializeSListHead(&_header);
	}
	~LockFreeStackMemoryPool()
	{
		while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header))) {
			::_aligned_free(memory);
		}
	}
public:
	void Push(MemoryHeader* ptr)
	{
		// ptr->allocSize = 0;
		::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));
	}

	MemoryHeader* Pop()
	{
		MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));
		if (memory == nullptr) {
			memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT));
		}
		else {
			if (memory->allocSize == 0) {
				abort();
			}
		}
		return memory;
	}
private:
	SLIST_HEADER _header;
	int _allocSize = 0;
};
