#pragma once
#include <vector>

class SMemoryBase
{
};

template<typename T>
class SMemory : public SMemoryBase
{
	static_assert(std::is_constructible<T, int>::value, "T must have constructor (T(int allocSize) must exist)");
protected:
	enum {
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};
public:
	SMemory()
	{
		int size = 0;
		int tableIndex = 0;
		for (size = 32; size <= 1024; size += 32) {
			T* pool = new T(size);
			_pools.push_back(pool);

			while (tableIndex <= size) {
				_poolTable[tableIndex] = pool;
				++tableIndex;
			}
		}

		for (; size <= 2048; size += 128) {
			T* pool = new T(size);
			_pools.push_back(pool);

			while (tableIndex <= size) {
				_poolTable[tableIndex] = pool;
				++tableIndex;
			}
		}

		for (; size <= 4096; size += 256) {
			T* pool = new T(size);
			_pools.push_back(pool);

			while (tableIndex <= size) {
				_poolTable[tableIndex] = pool;
				++tableIndex;
			}
		}
	}
	~SMemory()
	{
		for (T* pool : _pools)
			delete pool;

		_pools.clear();
	}

	virtual void* Allocate(int size) = 0;
	virtual void Release(void* ptr) = 0;
protected:
	std::vector<T*> _pools;
	T* _poolTable[MAX_ALLOC_SIZE + 1];
};
