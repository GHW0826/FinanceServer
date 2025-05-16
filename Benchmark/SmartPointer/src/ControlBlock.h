#pragma once
#include <atomic>
using namespace std;

template <typename T>
class ControlBlock {
public:
    ControlBlock(T* p)
        : _ptr(p)
        , _shared_count(1)
        , _weak_count(0)
    {
    }
private:
    T* _ptr;
    atomic<int> _shared_count;
    atomic<int> _weak_count;
};