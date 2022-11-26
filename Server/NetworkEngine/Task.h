#pragma once

#include "PoolObject.h"

class Task
{
private:
	function<void()> func;

public:
	Task(function<void()> _func) : func(std::move(_func)) {}

	void operator()() { func(); }
};

template<typename F, typename T, size_t ...Is>
static void __CallFunc(const F& func, T&& tupleArgs, std::index_sequence<Is...>)
{
	std::invoke(func, std::get<Is>(tupleArgs)...);
}

static TaskPtr MakeTask(std::function<void()> fIn)
{
	return std::make_unique<Task>(std::move(fIn));
}

template<typename F, typename ...Args>
static TaskPtr MakeTask(F&& fIn, Args&&... argsIn)
{
	return std::make_unique<Task>([f = std::forward<F>(fIn), tupleArgs = std::make_tuple(std::forward<Args>(argsIn)...)]()
	{
		using TupleType = decltype(tupleArgs);
		__CallFunc(f, tupleArgs, std::make_index_sequence<std::tuple_size_v<TupleType>>());
	});
}

template<typename R, typename C, typename ...A, typename ...Args>
static TaskPtr MakeTask(R(C::* mem_func)(A...), C* inst, Args&&... argsIn)
{
	return std::make_unique<Task>([mem_func, tupleArgs = std::make_tuple(inst, std::forward<Args>(argsIn)...)]() mutable
	{
		using TupleType = decltype(tupleArgs);
		__CallFunc(mem_func, tupleArgs, std::make_index_sequence<std::tuple_size_v<TupleType>>());
	});
}

template<typename R, typename C, typename ...A, typename ...Args>
static TaskPtr MakeTask(R(C::* mem_func)(A...), const std::shared_ptr<C>& inst, Args&&... argsIn)
{
	return std::make_unique<Task>([mem_func, tupleArgs = std::make_tuple(inst, std::forward<Args>(argsIn)...)]() mutable
	{
		using TupleType = decltype(tupleArgs);
		__CallFunc(mem_func, tupleArgs, std::make_index_sequence<std::tuple_size_v<TupleType>>());
	});
}