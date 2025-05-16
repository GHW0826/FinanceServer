
template <typename T>
class SharedPtr {
public:
    ControlBlock<T>* ctrl;  // 제어 블록 포인터 (공유/약한 카운트 및 객체 관리)
    
    // 생성자: 생 포인터로부터 SharedPtr 생성
    explicit SharedPtr(T* p = nullptr) : ctrl(nullptr) {
        if (p) {
            ctrl = new ControlBlock<T>(p);
            std::cout << "SharedPtr 생성: shared_count=" 
                      << ctrl->shared_count << "\n";
        }
    }
    // 복사 생성자
    SharedPtr(const SharedPtr& other) : ctrl(other.ctrl) {
        if (ctrl) {
            ctrl->shared_count += 1;
            std::cout << "SharedPtr 복사 생성: shared_count=" 
                      << ctrl->shared_count << "\n";
        }
    }
    // 이동 생성자
    SharedPtr(SharedPtr&& other) noexcept : ctrl(other.ctrl) {
        other.ctrl = nullptr;
        if (ctrl) {
            std::cout << "SharedPtr 이동 생성: shared_count=" 
                      << ctrl->shared_count << " (변화 없음)\n";
        }
    }
    // 소멸자
    ~SharedPtr() {
        if (ctrl) {
            ctrl->shared_count -= 1;
            std::cout << "SharedPtr 소멸: shared_count=" 
                      << ctrl->shared_count << "\n";
            if (ctrl->shared_count == 0) {
                // 공유 참조가 더 없다면 객체 삭제
                std::cout << "  shared_count=0, " 
                          << "객체 삭제\n";
                delete ctrl->ptr;
                ctrl->ptr = nullptr;
                if (ctrl->weak_count == 0) {
                    // 약한 참조도 없으면 제어 블록 삭제
                    std::cout << "  weak_count=0, 제어 블록 삭제\n";
                    delete ctrl;
                }
            }
        }
    }
    // 복사 대입 연산자
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            // 현재 가리키던 객체의 참조 해제
            if (ctrl) {
                ctrl->shared_count -= 1;
                std::cout << "SharedPtr 대입: 기존 shared_count=" 
                          << ctrl->shared_count << "\n";
                if (ctrl->shared_count == 0) {
                    std::cout << "  shared_count=0, 객체 삭제\n";
                    delete ctrl->ptr;
                    ctrl->ptr = nullptr;
                    if (ctrl->weak_count == 0) {
                        std::cout << "  weak_count=0, 제어 블록 삭제\n";
                        delete ctrl;
                    }
                }
            }
            // 다른 공유 블록 가리키도록 변경
            ctrl = other.ctrl;
            if (ctrl) {
                ctrl->shared_count += 1;
                std::cout << "SharedPtr 대입: 새로운 shared_count=" 
                          << ctrl->shared_count << "\n";
            }
        }
        return *this;
    }
    // 이동 대입 연산자
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            // 현재 가리키던 객체의 참조 해제
            if (ctrl) {
                ctrl->shared_count -= 1;
                std::cout << "SharedPtr 이동 대입: 기존 shared_count=" 
                          << ctrl->shared_count << "\n";
                if (ctrl->shared_count == 0) {
                    std::cout << "  shared_count=0, 객체 삭제\n";
                    delete ctrl->ptr;
                    ctrl->ptr = nullptr;
                    if (ctrl->weak_count == 0) {
                        std::cout << "  weak_count=0, 제어 블록 삭제\n";
                        delete ctrl;
                    }
                }
            }
            // 다른 공유 블록을 인수
            ctrl = other.ctrl;
            other.ctrl = nullptr;
            if (ctrl) {
                std::cout << "SharedPtr 이동 대입: shared_count=" 
                          << ctrl->shared_count << " (변화 없음)\n";
            }
        }
        return *this;
    }
    
    // 포인터처럼 접근하기 위한 연산자/메서드
    T& operator*() const { return *(ctrl->ptr); }
    T* operator->() const { return ctrl ? ctrl->ptr : nullptr; }
    T* get()   const { return ctrl ? ctrl->ptr : nullptr; }
    int use_count() const { return ctrl ? ctrl->shared_count : 0; }
    
    // 명시적 해제 (현재 객체 참조에서 손을 뗌)
    void reset() { *this = SharedPtr(); }
};
