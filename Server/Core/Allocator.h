#pragma once



class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

class StompAllocator
{
	enum { PAGE_SIZE = 0x1000};
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

class PoolAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
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
		// return static_cast<T*>(salloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
		// srelease(ptr);
	}


	template<typename U>
	bool operator==(const StlAllocator<U>&) { return true; }

	template<typename U>
	bool operator!=(const StlAllocator<U>&) { return false; }
};