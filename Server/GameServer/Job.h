#pragma once

/*
template<int... Remains>
struct seq
{};

template<int N, int... Remains>
struct gen_seq : gen_seq<N - 1, N - 1, Remains...>
{};

template<int... Remains>
struct gen_seq<0, Remains...> : seq<Remains...>
{};

template<typename Ret, typename... Args>
void sapply(Ret(*func)(Args...), std::tuple<Args...>& args)
{
	sapply_helper(func, gen_seq<sizeof...(Args)>(), args);
}

template<typename F, typename... Args, int... Is>
void sapply_helper(F func, seq<Is...>, std::tuple<Args...>& args)
{
	func(std::get<Is>(args)...);
}


template<typename T, typename Ret, typename... Args>
void sapply(T* obj, Ret(T::*func)(Args...), std::tuple<Args...>& args)
{
	sapply_helper(obj, func, gen_seq<sizeof...(Args)>(), args);
}

template<typename T, typename F, typename... Args, int... Is>
void sapply_helper(T* obj, F func, seq<Is...>, std::tuple<Args...>& args)
{
	(obj->func)(std::get<Is>(args)...);
}

class IJob
{
public:
	virtual void Execute() {}
};


template<typename Ret, typename... Args>
class FuncJob : public IJob
{
	using FuncType = Ret(*)(Args...);
public:
	FuncJob(FuncType func, Args... args)
		: _func(func), _tuple(args...)
	{
	}

	Ret operator()(Args... args)
	{
		std::apply(_func, _tuple);
		// _func(args);
	}

	virtual void Execute() override
	{

		// C++17
		std::apply(_func, _tuple);
		// _func(args);
	}


private:
	FuncType _func;
	std::tuple<Args...> _tuple;
};


template<typename T, typename Ret, typename... Args>
class MemberJob : public IJob
{
	using FuncType = Ret(T::*)(Args...);
public:
	FuncJob(T* obj, FuncType func, Args... args)
		: _obj(obj), _func(func), _tuple(args...)
	{
	}

	Ret operator()(Args... args)
	{
		std::apply(_func, _tuple);
		// _func(args);
	}

	virtual void Execute(Args... args) override
	{

		// C++17
		std::apply(_func, _tuple);
		// _func(args);
	}


private:
	T* _obj;
	FuncType _func;
	std::tuple<Args...> _tuple;
};


class HealJob : public IJob
{
public:
	virtual void Execute() override
	{
		cout << _target << " to heal: " << _healValue << endl;
	}
public:
	uint64 _target = 0;
	uint32 _healValue = 0;
};
*/