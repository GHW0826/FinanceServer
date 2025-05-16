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

    // ����, �̵�
    StdSharedPtr(const StdSharedPtr<T>& other) = default;
    StdSharedPtr(StdSharedPtr<T>&& other) noexcept = default;

    // ���� ����, �̵� ����
    StdSharedPtr<T>& operator=(const StdSharedPtr<T>& other) = default;
    StdSharedPtr<T>& operator=(StdSharedPtr<T>&& other) noexcept = default;

    // ���� ������ ����: get(), operator*, operator->
    T* get() const { return ptr.get(); }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr.get(); }

    // �Ҹ��� ��ȯ: non-null�̸� true
    operator bool() const { return ptr != nullptr; }

    // ����
    void reset() { ptr.reset(); }

    // ���� ī��Ʈ ��ȯ
    long use_count() const { return ptr.use_count(); }

    // ���� ������ ��ȯ
    void swap(StdSharedPtr<T>& other) { ptr.swap(other.ptr); }
private:
    std::shared_ptr<T> ptr;
};
