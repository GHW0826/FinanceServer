#pragma once
#include "Allocator.h"

class MemoryPool;

class Memory
{
	enum {
		// ~1024까지 32 단위, ~2048단위 까지 128단위, ~4096까지 256단위.
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256), 
		MAX_ALLOC_SIZE = 4096
	};
public:
	Memory();
	~Memory();
public:
	void* Allocate(int32 size);
	void Release(void* ptr);
private:
	std::vector<MemoryPool*> _pools;

	// 메모리 크기 <-> 메모리 풀
	// O(1) 빠르게 찾기 위한 테이블
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};


template<typename Type, typename... Args>
Type* snew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	new(memory)Type(forward<Args>(args)...); // placement new
	return memory;
}

template<typename Type>
void sdelete(Type* ptr)
{
	ptr->~Type();
	PoolAllocator::Release(ptr);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type> { snew<Type>(forward<Args>(args)...), sdelete<Type> };
}
