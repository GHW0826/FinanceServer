# SmartPointer
  
## 1. 개요
C++에서 스마트 포인터는 **RAII**를 바탕으로 동적 할당된 객체의 생명주기를 안전하게 관리하기 위해 제공.   
내부적으로 포인터와 포인터 생명 주기를 위해 관리하는 ControlBlock등이 있고, 유형에 따라 unique_ptr, shared_ptr, weak_ptr로 나뉜다.   

### 1. ControlBlock   
스마트 포인터 중 shared_ptr, weak_ptr는 Control Block이라는 별도의 관리 객체를 두고, 객체에 대한 참조 횟수(reference count)를 관리함.   
Control Block에는 다음과 같은 정보가 포함됩니다.   
- 포인터가 가리키는 실제 객체(리소스)의 주소   
- Use Count  (shared_ptr가 몇 개나 해당 객체를 소유한 지닌 스마트 포인터의 개수)   
- Weak Count (weak_ptr가 몇 개나 이 객체를 참조하고 있는지, 즉 약한 참조를 지닌 스마트 포인터의 개수)   

shared_ptr가 생성되면 Control Block이 생성되어 Use Count를 1로 설정, shared_ptr가 복사될 때마다 Use Count가 증가합니다.   
shared_ptr가 소멸되면 Use Count가 감소하여 0이 되는 시점에 실제 객체가 해제됩니다.   
weak_ptr는 객체가 해제될 때까지 약한 참조를 유지하고 있어도 소유권은 없으므로, weak_ptr 자체가 소멸하거나 감소해도 객체 해제 시점에 관여하지 않습니다.   


### 2. unique_ptr   
단일 소유권(Exclusive Ownership)을 가진 스마트 포인터
복사(copy)는 불가능하지만, 이동(move)는 가능함.
스코프를 벗어나거나, reset() 등을 통해 소멸될 때 관리 중인 객체도 함께 해제됨.

```cpp
#include <memory>

int main() {
    std::unique_ptr<int> uptr1 = std::make_unique<int>(10);
    // std::unique_ptr<int> uptr2 = uptr1; // 불가능(복사 불가)
    
    // 이동은 가능
    std::unique_ptr<int> uptr3 = std::move(uptr1);
    // 이제 uptr1은 nullptr을 가리키고, uptr3가 10을 관리

    return 0; // uptr3가 스코프에서 벗어나면서 10이 자동으로 해제
}
```

### 3. shared_ptr   
동일한 리소스를 여러 shared_ptr 객체가 참조 가능.   
내부적으로 Control Block에서 Use Count를 관리.   
Use Count가 0이 되면 리소스가 해제됨.
```cpp
#include <memory>
#include <iostream>

int main() {
    std::shared_ptr<int> sptr1 = std::make_shared<int>(20);
    std::cout << "sptr1 use_count: " << sptr1.use_count() << std::endl; // 1

    {
        std::shared_ptr<int> sptr2 = sptr1;
        std::cout << "sptr1 use_count: " << sptr1.use_count() << std::endl; // 2
        std::cout << "sptr2 use_count: " << sptr2.use_count() << std::endl; // 2
    }

    // sptr2가 스코프를 벗어나면서 use_count 감소
    std::cout << "sptr1 use_count: " << sptr1.use_count() << std::endl; // 1

    return 0; // sptr1 소멸 -> use_count=0 -> 리소스 해제
}
```



### 4. weak_ptr   
객체를 참조하되, 소유권은 갖지 않는 스마트 포인터.   
shared_ptr의 Control Block을 참조하지만, Use Count에 포함되지 않음.   
weak_ptr로 객체에 접근하려면 lock() 함수를 통해 shared_ptr를 획득한 뒤 사용.   
사용하기 전에 항상 expired()로 확인하거나 lock()을 통해 실제 객체를 안전하게 획득해야 함.   

```cpp
#include <memory>
#include <iostream>

struct Node {
    std::shared_ptr<Node> next;
    // 약한 참조로 순환 참조 방지
    std::weak_ptr<Node> prev;
    
    int value;
    Node(int val) : value(val) {}
};

int main() {
    auto node1 = std::make_shared<Node>(1);
    auto node2 = std::make_shared<Node>(2);

    node1->next = node2;
    node2->prev = node1; // 여기서 만약 prev를 std::shared_ptr<Node>로 하면 순환 참조 발생!

    // weak_ptr로 prev를 참조하므로, node1이 소멸되면 node2->prev.lock()은 nullptr을 반환
    if (auto p = node2->prev.lock()) {
        std::cout << "node2->prev value = " << p->value << std::endl;
    } else {
        std::cout << "node2->prev is expired" << std::endl;
    }

    return 0;
}
```
