#pragma once
#include <cstdlib>
#include <list>
#include <mutex>
#include <stdexcept>

class LockListMemoryPool
{
public:
    LockListMemoryPool(int allocSize)
        : _allocSize(allocSize)
    {
    }
    ~LockListMemoryPool()
    {
        for (auto& p : _freeList) {
            if (p) {
                delete p;
                p = nullptr;
            }
        }
        _freeList.clear();
    }
public:
    void Push(void* ptr)
    {
        std::lock_guard<std::mutex> lock(_m);
        _freeList.push_back(ptr);
    }

    void* Pop()
    {
        std::lock_guard<std::mutex> lock(_m);
        void* memory = nullptr;
        if (_freeList.empty()) {
            memory = ::malloc(_allocSize);
        }
        else {
            memory = _freeList.back();
            _freeList.pop_back();
        }
        return memory;
    }
private:
    std::mutex _m;
    int _allocSize = 0;
    std::list<void*> _freeList;
};
