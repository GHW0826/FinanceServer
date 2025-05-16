

#include <iostream>
#include <utility>
#include <type_traits>



//  6.1. Distinguishing prvalue / xvalue
template<typename T>
std::string valueCategory(T&&) {
    if constexpr (std::is_lvalue_reference_v<T>) {
        return "lvalue";
    } else {
        return "rvalue";
    }
}

// Overloads to see xvalue vs prvalue
// **Note**: At runtime “xvalue vs prvalue” both appear as `int&&` function overload.
// The standard library does not provide a built-in runtime function to differentiate xvalue from prvalue. They are both sub-kinds of rvalue. 
// The difference is more relevant at compile time (particularly for overload resolution and certain corner cases).
void checkCategory(const int&) { std::cout << "lvalue\n"; }
void checkCategory(int&&)      { std::cout << "rvalue\n"; }

int prvalue_xvalue()
{
    int x = 10;
    checkCategory(x);         // lvalue
    checkCategory(std::move(x)); // xvalue but from code's perspective calls int&& => rvalue

    // prvalue example
    checkCategory(42);        // prvalue => int&& => prints "rvalue"

    return 0;
}


// 6.2. Observing move vs copy with prvalue / xvalue
struct Tracer {
    Tracer() { std::cout << "Default ctor\n"; }
    Tracer(const Tracer&) { std::cout << "Copy ctor\n"; }
    Tracer(Tracer&&) { std::cout << "Move ctor\n"; }
};

Tracer makeTracerPRValue() {
    return Tracer(); // prvalue
}

// **C++17** may do guaranteed RVO so you might see no “move” or “copy.” 
// But disabling optimization or using older standards might show “Move ctor.”
int prvalue_xvalue()
{
    std::cout << "-- prvalue test --\n";
    Tracer t1 = makeTracerPRValue(); // Usually no move or copy if RVO, but if not optimized, it's "Move ctor" (C++17 guaranteed elision though)

    std::cout << "-- xvalue test --\n";
    Tracer t2;
    Tracer t3 = static_cast<Tracer&&>(t2); // forcibly xvalue => calls Move ctor
    return 0;
}





