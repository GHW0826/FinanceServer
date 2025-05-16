#pragma once


#define OUT

/*
// Memory
#ifdef _DEBUG
#define salloc(size) StompAllocator::Alloc(size)
#define srelease(ptr) StompAllocator::Release(ptr)

//#define salloc(size) PoolAllocator::Alloc(size)
//#define srelease(ptr) PoolAllocator::Release(ptr)
#else
#define salloc(size) BaseAllocator::Alloc(size)
#define srelease(ptr) BaseAllocator::Release(ptr)
#endif
*/

// #define salloc(size) PoolAllocator::Alloc(size)
// #define srelease(ptr) PoolAllocator::Release(ptr)

// Lock
#define USE_MANY_LOCK(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCK(1);
#define READ_LOCK_IDX(idx)		SReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0);
#define WRITE_LOCK_IDX(idx)		SWriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0);

// Crash
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
}


#define RAW_POINTER_NEW(name, type)		\
{										\
	if (name == nullptr)				\
		name = new type;				\
}										\

#define RAW_POINTER_DELETE(name)		\
{										\
	if (name != nullptr) {				\
		delete name;					\
		name = nullptr;					\
	}									\
}										\

#if __cplusplus >= 202002L
    #define CPP_VERSION 20
#elif __cplusplus >= 201703L
    #define CPP_VERSION 17
#elif __cplusplus >= 201402L
    #define CPP_VERSION 14
#elif __cplusplus >= 201103L
    #define CPP_VERSION 11
#else
    #define CPP_VERSION 98  // 보통 C++98을 legacy C++로 봅니다. 199711L
#endif

#if __has_include(<boost/version.hpp>)
    #define HAS_BOOST 1
#else
    #define HAS_BOOST 0
#endif


#define NAMESPACE_BEGIN(name)	namespace name {
#define NAMESPACE_END			}

