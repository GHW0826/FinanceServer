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
		_callback = [owner, memFunc, args...]() {
			(owner.get()->*memFunc)(args...);
		};
	}
public:
	void Execute()
	{
		_callback();
	}
private:
	Callback _callback;
};

