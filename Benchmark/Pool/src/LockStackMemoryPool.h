#pragma once
#include <cstdlib>
#include <stack>
#include <mutex>
#include <stdexcept>

class LockStackMemoryPool
{
public:
    LockStackMemoryPool(int allocSize)
        : _allocSize(allocSize)
    {
    }
    ~LockStackMemoryPool()
    {
        std::lock_guard<std::mutex> lock(_m);
        while (_freeStack.empty() == false) {
            void* ptr = _freeStack.top();
            _freeStack.pop();
            delete(ptr);
            ptr = nullptr;
        }
    }
public:
    void Push(void* ptr)
    {
        std::lock_guard<std::mutex> lock(_m);
        _freeStack.push(ptr);
    }
    // 할당
    void* Pop()
    {
        std::lock_guard<std::mutex> lock(_m);
        void* memory = nullptr;
        if (_freeStack.empty()) {
            memory = ::malloc(_allocSize);
        }
        else {
            memory = _freeStack.top();
            _freeStack.pop();
        }
        return memory;
    }
private:
    std::mutex _m;
    int _allocSize = 0;
    std::stack<void*> _freeStack;
};

