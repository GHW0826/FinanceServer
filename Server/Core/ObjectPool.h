#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename ... Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(SAllocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, SAllocSize));
#else
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(SPool.Pop(), SAllocSize));
#endif
		new(memory)Type(forward<Args>(args)...);
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		SPool.Push(MemoryHeader::DetachHeader(obj));
#endif
	}

	template<typename... Args>
	static shared_ptr<Type> MakeShared(Args&&... args)
	{
		shared_ptr<Type> ptr = { Pop(forward<Args>(args)...), Push };
		return ptr;
	}
private:
	static int32 SAllocSize;
	static MemoryPool SPool;
};


template<typename Type>
int32 ObjectPool<Type>::SAllocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::SPool{ SAllocSize };