#pragma once
#include <memory>

template<class T>
class EnableSharedFromThis {
protected:
    constexpr EnableSharedFromThis() {}
    EnableSharedFromThis(EnableSharedFromThis const&) {}
    EnableSharedFromThis& operator=(EnableSharedFromThis const&) {
        return *this;
    }

public:
    std::shared_ptr<T> shared_from_this() { return self_.lock(); }
    std::shared_ptr<T const> shared_from_this() const { return self_.lock(); }

private:
    std::weak_ptr<T> self_;

    friend std::shared_ptr<t>;
};
