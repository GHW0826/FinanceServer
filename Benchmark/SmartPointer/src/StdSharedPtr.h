#pragma once
#include <iostream>
#include <memory>
#include <string>

template<typename T>
class StdSharedPtr {
public:
    StdSharedPtr() 
        : ptr(nullptr)
    {
    }
    explicit StdSharedPtr(T* raw_ptr) 
        : ptr(raw_ptr) 
    {
    }

    explicit StdSharedPtr(std::shared_ptr<T> sp) 
        : ptr(std::move(sp)) 
    {
    }

    // 복사, 이동
    StdSharedPtr(const StdSharedPtr<T>& other) = default;
    StdSharedPtr(StdSharedPtr<T>&& other) noexcept = default;

    // 복사 대입, 이동 대입
    StdSharedPtr<T>& operator=(const StdSharedPtr<T>& other) = default;
    StdSharedPtr<T>& operator=(StdSharedPtr<T>&& other) noexcept = default;

    // 내부 포인터 접근: get(), operator*, operator->
    T* get() const { return ptr.get(); }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr.get(); }

    // 불리언 변환: non-null이면 true
    operator bool() const { return ptr != nullptr; }

    // 리셋
    void reset() { ptr.reset(); }

    // 참조 카운트 반환
    long use_count() const { return ptr.use_count(); }

    // 내부 포인터 교환
    void swap(StdSharedPtr<T>& other) { ptr.swap(other.ptr); }
private:
    std::shared_ptr<T> ptr;
};
