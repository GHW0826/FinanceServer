#pragma once
#include "ControlBlock.h"

template <typename T>
class SharedPtr {
public:
    ControlBlock<T>* ctrl;

    // ������: �� �����ͷκ��� SharedPtr ����
    explicit SharedPtr(T* p = nullptr) 
        : ctrl(nullptr) 
    {
        if (p) {
            ctrl = new ControlBlock<T>(p);
            std::cout << "SharedPtr ����: shared_count=" << ctrl->shared_count << "\n";
        }
    }
    // ���� ������
    SharedPtr(const SharedPtr& other) 
        : ctrl(other.ctrl) 
    {
        if (ctrl) {
            ctrl->shared_count += 1;
            std::cout << "SharedPtr ���� ����: shared_count=" << ctrl->shared_count << "\n";
        }
    }
    // �̵� ������
    SharedPtr(SharedPtr&& other) noexcept 
        : ctrl(other.ctrl) 
    {
        other.ctrl = nullptr;
        if (ctrl) {
            std::cout << "SharedPtr �̵� ����: shared_count=" << ctrl->shared_count << " (��ȭ ����)\n";
        }
    }
    // �Ҹ���
    ~SharedPtr() {
        if (ctrl) {
            ctrl->shared_count -= 1;
            std::cout << "SharedPtr �Ҹ�: shared_count=" << ctrl->shared_count << "\n";
            if (ctrl->shared_count == 0) {
                // ���� ������ �� ���ٸ� ��ü ����
                std::cout << "  shared_count=0, " << "��ü ����\n";
                delete ctrl->ptr;
                ctrl->ptr = nullptr;
                if (ctrl->weak_count == 0) {
                    // ���� ������ ������ ���� ��� ����
                    std::cout << "  weak_count=0, ���� ��� ����\n";
                    delete ctrl;
                }
            }
        }
    }
    // ���� ���� ������
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            // ���� ����Ű�� ��ü�� ���� ����
            if (ctrl) {
                ctrl->shared_count -= 1;
                std::cout << "SharedPtr ����: ���� shared_count=" << ctrl->shared_count << "\n";
                if (ctrl->shared_count == 0) {
                    std::cout << "  shared_count=0, ��ü ����\n";
                    delete ctrl->ptr;
                    ctrl->ptr = nullptr;
                    if (ctrl->weak_count == 0) {
                        std::cout << "  weak_count=0, ���� ��� ����\n";
                        delete ctrl;
                    }
                }
            }
            // �ٸ� ���� ��� ����Ű���� ����
            ctrl = other.ctrl;
            if (ctrl) {
                ctrl->shared_count += 1;
                std::cout << "SharedPtr ����: ���ο� shared_count=" << ctrl->shared_count << "\n";
            }
        }
        return *this;
    }
    // �̵� ���� ������
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            // ���� ����Ű�� ��ü�� ���� ����
            if (ctrl) {
                ctrl->shared_count -= 1;
                std::cout << "SharedPtr �̵� ����: ���� shared_count=" << ctrl->shared_count << "\n";
                if (ctrl->shared_count == 0) {
                    std::cout << "  shared_count=0, ��ü ����\n";
                    delete ctrl->ptr;
                    ctrl->ptr = nullptr;
                    if (ctrl->weak_count == 0) {
                        std::cout << "  weak_count=0, ���� ��� ����\n";
                        delete ctrl;
                    }
                }
            }
            // �ٸ� ���� ����� �μ�
            ctrl = other.ctrl;
            other.ctrl = nullptr;
            if (ctrl) {
                std::cout << "SharedPtr �̵� ����: shared_count=" << ctrl->shared_count << " (��ȭ ����)\n";
            }
        }
        return *this;
    }

    // ������ó�� �����ϱ� ���� ������/�޼���
    T& operator*() const { return *(ctrl->ptr); }
    T* operator->() const { return ctrl ? ctrl->ptr : nullptr; }
    T* get()   const { return ctrl ? ctrl->ptr : nullptr; }
    int use_count() const { return ctrl ? ctrl->shared_count : 0; }

    // ����� ���� (���� ��ü �������� ���� ��)
    void reset() { *this = SharedPtr(); }
};
