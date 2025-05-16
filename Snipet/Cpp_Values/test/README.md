
```cpp
#include <iostream>
#include <utility>
#include <string>
#include <type_traits>

// 1. 함수 오버로드: lvalue와 rvalue 구분
// lvalue 버전: const lvalue reference
void process(const std::string& s) {
    std::cout << "lvalue overload process(const std::string&): " << s << "\n";
}

// rvalue 버전: rvalue reference
void process(std::string&& s) {
    std::cout << "rvalue overload process(std::string&&): " << s << "\n";
}

// 2. std::move 예제: lvalue를 rvalue로 캐스팅하여 이동시키기
void demonstrateMove() {
    std::string str = "Hello, Move!";
    std::cout << "\n[demonstrateMove] Before move, str: " << str << "\n";
    // std::move는 단순히 캐스팅만 하므로, str의 값은 그대로 유지됨.
    process(std::move(str));  // rvalue overload 호출
    std::cout << "[demonstrateMove] After move, str: " << str << "\n";
}

// 3. 보편참조(Universal/Forwarding Reference)와 std::forward 예제
//  Perfect Forwarding을 보여주는 템플릿 함수
// T&& 인자는 함수 템플릿에서 전달된 값의 카테고리를 그대로 보존합니다.
// T&&는 universal reference로, 인자로 전달된 값의 lvalue/rvalue 성질을 보존합니다.
template <typename T>
void forwardingFunction(T&& arg) {
    std::cout << "\n[forwardingFunction] Forwarding argument... ";
    // std::forward<T>는 arg의 원래 lvalue/rvalue 성격을 그대로 전달합니다.
    // std::forward<T>(param)를 사용하면 param의 원래 값 범주(value category)를 유지하면서 전달됩니다.
    process(std::forward<T>(arg));
}

// 2. Move semantics를 보여주는 클래스
class MyString {
public:
    // 일반 생성자
    MyString(const std::string& str) : data(new std::string(str)) {
        std::cout << "Constructed: " << *data << std::endl;
    }
    // 복사 생성자 (deep copy)
    MyString(const MyString& other) : data(new std::string(*other.data)) {
        std::cout << "Copied: " << *data << std::endl;
    }
    // 이동 생성자 (move semantics)
    MyString(MyString&& other) noexcept : data(other.data) {
        other.data = nullptr;
        std::cout << "Moved" << std::endl;
    }
    // 소멸자
    ~MyString() {
        if (data) {
            std::cout << "Destroyed: " << *data << std::endl;
        } else {
            std::cout << "Destroyed: null" << std::endl;
        }
        delete data;
    }
private:
    std::string* data;
};


// 4. Move semantics를 보여주는 클래스 예제
class MyType {
public:
    std::string data;

    // 일반 생성자
    MyType(const std::string& s) : data(s) {
        std::cout << "MyType copy constructor: " << data << "\n";
    }

    // 이동 생성자
    MyType(std::string&& s) : data(std::move(s)) {
        std::cout << "MyType move constructor: " << data << "\n";
    }

    // 복사 대입 연산자
    MyType& operator=(const MyType& other) {
        if (this != &other) {
            data = other.data;
            std::cout << "MyType copy assignment: " << data << "\n";
        }
        return *this;
    }

    // 이동 대입 연산자
    MyType& operator=(MyType&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            std::cout << "MyType move assignment: " << data << "\n";
        }
        return *this;
    }
};

// 팩토리 함수: 반환 시 이동 생성자가 호출됨
MyType createMyType() {
    return MyType("Temporary MyType");
}

int main() {
    // --- lvalue와 rvalue 예제 ---
    std::cout << "=== lvalue vs. rvalue overload test ===\n";
    std::string hello = "Hello, World!";
    process(hello);                        // lvalue 버전 호출
    process(std::string("Temp String"));   // rvalue 버전 호출

    // --- std::move 예제 ---
    demonstrateMove();

    // --- 보편참조와 std::forward 예제 ---
    std::cout << "\n=== Perfect forwarding test ===\n";
    forwardingFunction(hello);             // lvalue로 전달 → lvalue overload 호출 inside process
    forwardingFunction(std::string("Forwarded Temp"));  // rvalue 전달 → rvalue overload

    // --- 람다를 이용한 보편참조 예제 ---
    std::cout << "\n=== Lambda forwarding example ===\n";
    auto lambda = [](auto&& param) {
        std::cout << "Lambda: ";
        process(std::forward<decltype(param)>(param));
    };
    lambda(hello);                         // lvalue → lvalue overload
    lambda(std::string("Lambda Rvalue"));  // rvalue → rvalue overload

    // --- Move semantics 테스트 ---
    std::cout << "\n=== Move semantics test with MyType ===\n";
    MyType a("MyType A");
    MyType b = a;              // 복사 생성자 호출
    MyType c = std::move(a);     // 이동 생성자 호출
    MyType d = createMyType();   // 반환 값 최적화(move elision) 또는 이동 생성자 호출

    return 0;
}
```



## 5.1. Lvalue vs Rvalue Test

```cpp
#include <iostream>
#include <string>
#include <utility>

void foo(const std::string& s) {
    std::cout << "foo(const std::string&) lvalue\n";
}
void foo(std::string&& s) {
    std::cout << "foo(std::string&&) rvalue\n";
}

int main()
{
    std::string s = "Hello";
    foo(s);               // calls foo(const std::string&) => lvalue
    foo(std::string("X")); // calls foo(std::string&&) => rvalue
    foo(std::move(s));    // also rvalue => "foo(std::string&&)"
    return 0;
}
```

- `s`은 lvalue, `"X"`는 임시 rvalue, `std::move(s)`도 rvalue

## 5.2. Move vs Copy

```cpp
#include <iostream>
#include <vector>

struct MyData {
    MyData() { std::cout << "MyData default ctor\n"; }
    MyData(const MyData&) { std::cout << "MyData copy ctor\n"; }
    MyData(MyData&&) { std::cout << "MyData move ctor\n"; }
};

int main()
{
    MyData d;
    std::cout << "push_back lvalue:\n";
    std::vector<MyData> v;
    v.push_back(d); // copy

    std::cout << "push_back rvalue:\n";
    v.push_back(std::move(d)); // move
    return 0;
}
```

- 관찰: “copy ctor” vs. “move ctor” 메시지 차이

## 5.3. Perfect Forwarding

```cpp

// `wrapper(a)`에서 T deduce → `int&`, => T&& => `int& &&` => `int&`, => `std::forward<T>(arg)` => lvalue.  
// `wrapper(20)` => T = int => T&& => int&& => rvalue.
#include <iostream>
#include <utility>

void bar(const int& x) { std::cout << "bar(const int&) - lvalue\n"; }
void bar(int&& x)      { std::cout << "bar(int&&) - rvalue\n"; }

template<typename T>
void wrapper(T&& arg) {
    // Perfect forwarding
    bar(std::forward<T>(arg));
}

int main()
{
    int a = 10;
    wrapper(a);         // bar(const int&) => lvalue
    wrapper(20);        // bar(int&&) => rvalue
    return 0;
}
```


