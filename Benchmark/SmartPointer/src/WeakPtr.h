#pragma once
#include "ControlBlock.h"

template <typename T>
class WeakPtr {
public:
    ControlBlock<T>* ctrl;

    WeakPtr() 
        : ctrl(nullptr) 
    {
    }
    WeakPtr(const SharedPtr<T>& sp) 
        : ctrl(sp.ctrl) 
    {
        if (ctrl) {
            ctrl->weak_count += 1;
            std::cout << "WeakPtr ����: weak_count=" << ctrl->weak_count << "\n";
        }
    }
    // ���� ������
    WeakPtr(const WeakPtr& other) 
        : ctrl(other.ctrl) 
    {
        if (ctrl) {
            ctrl->weak_count += 1;
            std::cout << "WeakPtr ���� ����: weak_count=" << ctrl->weak_count << "\n";
        }
    }
    // �̵� ������
    WeakPtr(WeakPtr&& other) noexcept 
        : ctrl(other.ctrl) 
    {
        other.ctrl = nullptr;
        if (ctrl) {
            std::cout << "WeakPtr �̵� ����: weak_count=" << ctrl->weak_count << " (��ȭ ����)\n";
        }
    }
    // �Ҹ���
    ~WeakPtr() {
        if (ctrl) {
            ctrl->weak_count -= 1;
            std::cout << "WeakPtr �Ҹ�: weak_count=" << ctrl->weak_count << "\n";
            if (ctrl->weak_count == 0 && ctrl->shared_count == 0) {
                // �� �̻� � ������ ���� �� ���� ��� ����
                std::cout << "  weak_count=0 && shared_count=0, ���� ��� ����\n";
                delete ctrl;
                ctrl = nullptr;
            }
        }
    }
    // ���� ���� ������
    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            // ���� ���� ���� ����
            if (ctrl) {
                ctrl->weak_count -= 1;
                std::cout << "WeakPtr ����: ���� weak_count=" << ctrl->weak_count << "\n";
                if (ctrl->weak_count == 0 && ctrl->shared_count == 0) {
                    std::cout << "  weak_count=0 && shared_count=0, ���� ��� ����\n";
                    delete ctrl;
                    ctrl = nullptr;
                }
            }
            // ���ο� ���� ���� ����Ŵ
            ctrl = other.ctrl;
            if (ctrl) {
                ctrl->weak_count += 1;
                std::cout << "WeakPtr ����: ���ο� weak_count=" << ctrl->weak_count << "\n";
            }
        }
        return *this;
    }
    // �̵� ���� ������
    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            if (ctrl) {
                ctrl->weak_count -= 1;
                std::cout << "WeakPtr �̵� ����: ���� weak_count=" << ctrl->weak_count << "\n";
                if (ctrl->weak_count == 0 && ctrl->shared_count == 0) {
                    std::cout << "  weak_count=0 && shared_count=0, ���� ��� ����\n";
                    delete ctrl;
                    ctrl = nullptr;
                }
            }
            ctrl = other.ctrl;
            other.ctrl = nullptr;
            if (ctrl) {
                std::cout << "WeakPtr �̵� ����: weak_count=" << ctrl->weak_count << " (��ȭ ����)\n";
            }
        }
        return *this;
    }
    // lock(): ���� �����Ͱ� ����Ű�� ��ü�� SharedPtr�� ȹ��
    SharedPtr<T> lock() const {
        if (ctrl && ctrl->shared_count > 0) {
            // ��ü�� ���� ����ִٸ� SharedPtr ����
            SharedPtr<T> sp;
            sp.ctrl = ctrl;
            ctrl->shared_count += 1;
            std::cout << "WeakPtr lock: ��ü ����, shared_count=" << ctrl->shared_count << "\n";
            return sp;
        }
        else {
            std::cout << "WeakPtr lock: ��ü �Ҹ��, �� SharedPtr ��ȯ\n";
            return SharedPtr<T>();
        }
    }
    // ���� ���� Ȯ��
    bool expired() const {
        return !ctrl || ctrl->shared_count == 0;
    }
};
