#pragma once
#include "ITestMemoryPool.h"
#include <mutex>

template<typename T>
class RawPointerPool : public ITestMemoryPool<T> {
public:
    RawPointerPool(size_t count) 
        : _count(count), _index(count) 
    {
        _pool = static_cast<T**>(malloc(sizeof(T*) * count));
        for (size_t i = 0; i < count; ++i)
            _pool[i] = reinterpret_cast<T*>(::operator new(sizeof(T)));
    }

    ~RawPointerPool() 
    {
        for (size_t i = 0; i < _count; ++i) {
            ::operator delete(_pool[i]);
        }
        free(_pool);
    }

    T* Allocate() override 
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_index == 0) {
            T* p = reinterpret_cast<T*>(::operator new(sizeof(T)));
            expand();
            _pool[_index++] = p;
            return p;
        }
        return _pool[--_index];
    }

    void Deallocate(T* ptr) override
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_index == _capacity)
            expand();
        _pool[_index++] = ptr;
    }

private:
    void expand() {
        size_t newCapacity = (_capacity == 0) ? 64 : _capacity * 2;
        T** newPool = static_cast<T**>(malloc(sizeof(T*) * newCapacity));
        for (size_t i = 0; i < _index; ++i)
            newPool[i] = _pool[i];
        free(_pool);
        _pool = newPool;
        _capacity = newCapacity;
    }

    T** _pool = nullptr;
    size_t _count;
    size_t _index;
    size_t _capacity = 0;
    std::mutex _mutex;
};
