#pragma once

template <typename T>
class ControlBlock {
public:
    ControlBlock(T* p)
        : _ptr(p)
        , _shared_count(1)
        , _weak_count(1)
    {
    }
private:
    T* _ptr;
    int _shared_count;
    int _weak_count;
};