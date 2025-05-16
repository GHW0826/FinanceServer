#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <vector>
#include <cassert>

enum class TypeTag {
    BASE,
    DERIVED
};

struct Base {
    virtual ~Base() {}
    virtual TypeTag getType() const { return TypeTag::BASE; }
};

struct Derived : public Base {
    int value = 42;
    TypeTag getType() const override { return TypeTag::DERIVED; }
};

constexpr int ITERATIONS = 10'000'000;

//---------------------------------------------
// 벤치마크 함수들
//---------------------------------------------

void benchmark_static_upcast() {
    std::shared_ptr<Derived> d = std::make_shared<Derived>();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        std::shared_ptr<Base> b = std::static_pointer_cast<Base>(d);
        (void)b;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[static_pointer_cast<Base>] Upcast elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
}

void benchmark_dynamic_downcast_success() {
    std::shared_ptr<Base> b = std::make_shared<Derived>();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        std::shared_ptr<Derived> d = std::dynamic_pointer_cast<Derived>(b);
        assert(d); // 항상 성공
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[dynamic_pointer_cast<Derived>] Downcast elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
}

void benchmark_static_downcast_with_typecheck() {
    std::shared_ptr<Base> b = std::make_shared<Derived>();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        if (b->getType() == TypeTag::DERIVED) {
            std::shared_ptr<Derived> d = std::static_pointer_cast<Derived>(b);
            (void)d;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[static_pointer_cast with tag check] elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
}

void benchmark_static_downcast_with_assert_check() {
    std::shared_ptr<Base> b = std::make_shared<Derived>();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        assert(std::dynamic_pointer_cast<Derived>(b)); // 디버깅용 체크
        std::shared_ptr<Derived> d = std::static_pointer_cast<Derived>(b);
        (void)d;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[static_pointer_cast with assert(dynamic)] elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
}

//---------------------------------------------
// 실행
//---------------------------------------------

int main() {
    benchmark_static_upcast();
    benchmark_dynamic_downcast_success();
    benchmark_static_downcast_with_typecheck();
    benchmark_static_downcast_with_assert_check();
    return 0;
}
