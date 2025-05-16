template <typename T>
class WeakPtr {
public:
    ControlBlock<T>* ctrl;
    
    // 생성자
    WeakPtr() : ctrl(nullptr) {}
    // SharedPtr로부터 약한 포인터 생성
    WeakPtr(const SharedPtr<T>& sp) : ctrl(sp.ctrl) {
        if (ctrl) {
            ctrl->weak_count += 1;
            std::cout << "WeakPtr 생성: weak_count=" 
                      << ctrl->weak_count << "\n";
        }
    }
    // 복사 생성자
    WeakPtr(const WeakPtr& other) : ctrl(other.ctrl) {
        if (ctrl) {
            ctrl->weak_count += 1;
            std::cout << "WeakPtr 복사 생성: weak_count=" 
                      << ctrl->weak_count << "\n";
        }
    }
    // 이동 생성자
    WeakPtr(WeakPtr&& other) noexcept : ctrl(other.ctrl) {
        other.ctrl = nullptr;
        if (ctrl) {
            std::cout << "WeakPtr 이동 생성: weak_count=" 
                      << ctrl->weak_count << " (변화 없음)\n";
        }
    }
    // 소멸자
    ~WeakPtr() {
        if (ctrl) {
            ctrl->weak_count -= 1;
            std::cout << "WeakPtr 소멸: weak_count=" 
                      << ctrl->weak_count << "\n";
            if (ctrl->weak_count == 0 && ctrl->shared_count == 0) {
                // 더 이상 어떤 참조도 없을 때 제어 블록 삭제
                std::cout << "  weak_count=0 && shared_count=0, 제어 블록 삭제\n";
                delete ctrl;
            }
        }
    }
    // 복사 대입 연산자
    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            // 기존 약한 참조 해제
            if (ctrl) {
                ctrl->weak_count -= 1;
                std::cout << "WeakPtr 대입: 기존 weak_count=" 
                          << ctrl->weak_count << "\n";
                if (ctrl->weak_count == 0 && ctrl->shared_count == 0) {
                    std::cout << "  weak_count=0 && shared_count=0, 제어 블록 삭제\n";
                    delete ctrl;
                }
            }
            // 새로운 약한 참조 가리킴
            ctrl = other.ctrl;
            if (ctrl) {
                ctrl->weak_count += 1;
                std::cout << "WeakPtr 대입: 새로운 weak_count=" 
                          << ctrl->weak_count << "\n";
            }
        }
        return *this;
    }
    // 이동 대입 연산자
    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            if (ctrl) {
                ctrl->weak_count -= 1;
                std::cout << "WeakPtr 이동 대입: 기존 weak_count=" 
                          << ctrl->weak_count << "\n";
                if (ctrl->weak_count == 0 && ctrl->shared_count == 0) {
                    std::cout << "  weak_count=0 && shared_count=0, 제어 블록 삭제\n";
                    delete ctrl;
                }
            }
            ctrl = other.ctrl;
            other.ctrl = nullptr;
            if (ctrl) {
                std::cout << "WeakPtr 이동 대입: weak_count=" 
                          << ctrl->weak_count << " (변화 없음)\n";
            }
        }
        return *this;
    }
    // lock(): 약한 포인터가 가리키는 객체를 SharedPtr로 획득
    SharedPtr<T> lock() const {
        if (ctrl && ctrl->shared_count > 0) {
            // 객체가 아직 살아있다면 SharedPtr 생성
            SharedPtr<T> sp;
            sp.ctrl = ctrl;
            ctrl->shared_count += 1;
            std::cout << "WeakPtr lock: 객체 생존, shared_count=" 
                      << ctrl->shared_count << "\n";
            return sp;
        } else {
            std::cout << "WeakPtr lock: 객체 소멸됨, 빈 SharedPtr 반환\n";
            return SharedPtr<T>();
        }
    }
    // 만료 여부 확인
    bool expired() const {
        return !ctrl || ctrl->shared_count == 0;
    }
};
