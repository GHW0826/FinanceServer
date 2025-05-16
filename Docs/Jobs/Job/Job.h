#pragma once

#include <functional>

using Callback = std::function<void()>;

class Job
{
public:
	Job(Callback&& callback)
		: _callback(std::move(callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		// owner.get() → shared_ptr<T>에서 T* 생포인터 추출
		// ->* memFunc → 멤버 함수 포인터를 호출
		// args... → 넘긴 인자들을 그대로 forward
		/* 
			T* ptr = owner.get();
			(ptr->*memFunc)(args...);
		*/
		_callback = [owner, memFunc, args...]() {
			(owner.get()->*memFunc)(args...);
		};
	}

	void Execute()
	{
		_callback();
	}
private:
	Callback _callback;
};

