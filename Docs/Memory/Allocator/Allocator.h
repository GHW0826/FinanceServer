#pragma once

class BaseAllocator
{
public:
	static void* Alloc(int32 size)
	{
		return ::malloc(size);
	}
	static void Release(void* ptr)
	{
		::free(ptr);
	}
};

class StompAllocator
{
	enum { PAGE_SIZE = 0x1000};
public:
	static void* Alloc(int32 size)
	{
		const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
		const int64 dataOffset = pageCount * PAGE_SIZE - size;
		void* baseAddress = ::VirtualAlloc(nullptr, pageCount * PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
	}
	static void Release(void* ptr)
	{
		const int64 address = reinterpret_cast<int64>(ptr);
		const int64 baseAddress = address - (address % PAGE_SIZE);
		::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
	}
};

class PoolAllocator
{
public:
	static void* Alloc(int32 size)
	{
		return GMemory->Allocate(size);
	}

	static void Release(void* ptr)
	{
		GMemory->Release(ptr);
	}
};

template<typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() {}

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

	T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
	}

	template<typename U>
	bool operator==(const StlAllocator<U>&) { return true; }

	template<typename U>
	bool operator!=(const StlAllocator<U>&) { return false; }
};
