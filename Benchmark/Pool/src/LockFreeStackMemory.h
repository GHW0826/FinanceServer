#pragma once
#include "SMemory.h"
#include "LockFreeStackMemoryPool.h"

class LockFreeStackMemory : public SMemory<LockFreeStackMemoryPool>
{
public:
	void* Allocate(int size) override
	{
		MemoryHeader* header = nullptr;
		const int allocSize = size + sizeof(MemoryHeader);
		if (allocSize > MAX_ALLOC_SIZE) {
			header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
		}
		else {
			header = _poolTable[allocSize]->Pop();
		}

		return MemoryHeader::AttachHeader(header, allocSize);
	}

	void Release(void* ptr) override
	{
		MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
		const int allocSize = header->allocSize;
		if (allocSize < 0) {
			abort();
		}
		if (allocSize > MAX_ALLOC_SIZE) {
			::_aligned_free(header);
		}
		else {
			_poolTable[allocSize]->Push(header);
		}
	}
};
