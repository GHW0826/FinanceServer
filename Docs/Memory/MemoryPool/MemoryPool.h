#pragma once

enum {
	SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	// [MemoryHeader][Data]
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize)
		: _allocSize(allocSize)
	{
		::InitializeSListHead(&_header);
	}

	~MemoryPool()
	{
		while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
			::_aligned_free(memory);
	}

	void Push(MemoryHeader* ptr)
	{
		ptr->allocSize = 0;

		::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));
	}
	MemoryHeader* Pop()
	{
		MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));
		if (memory == nullptr) {
			memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT));
		}
		else {
			ASSERT_CRASH(memory->allocSize == 0);
		}

		return memory;
	}
private:
	SLIST_HEADER _header;
	int32 _allocSize = 0;
};

