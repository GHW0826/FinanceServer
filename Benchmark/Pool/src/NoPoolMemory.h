#pragma once
#include "SMemory.h"
#include "ITestMemoryPool.h"

class NoPool
{
public:
    NoPool(int allocSize)
        : _allocSize(allocSize)
    {
    }
    ~NoPool() = default;
private:
    int _allocSize;
};

class NoPoolMemory : public SMemory<NoPool>
{
public:
    void* Allocate(int size) override
    {
        return ::malloc(size);
    }

    void Release(void* ptr) override
    {
        if (ptr != nullptr) {
            delete (ptr);
            ptr = nullptr;
        }
    }
};
