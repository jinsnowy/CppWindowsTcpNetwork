#pragma once

#include "ITaskCreator.h"

enum class TaskPriority
{
	High,
	Normal,
};

class TaskScheduler;
class TaskSchedule;
class TaskAsync
{
	friend class TaskSchedule;
private:
	class TaskAsyncImpl
	{
	public:
		TaskScheduler*   scheduler;
		StdMutex	     _mtx;
		queue<TaskPtr>   _tasks;

	public:
		TaskAsyncImpl();
		virtual ~TaskAsyncImpl();

		void Push(TaskPtr&& task);
		virtual void Submit() abstract;
	};

	TaskAsyncImpl* _impl;
	TaskAsync(TaskAsyncImpl* impl);

	static TaskAsync Create(TaskPriority priority)
	{
		static TaskAsyncHighPriortiyImpl highImpl;
		static TaskAsyncNormalPriortiyImpl normalImpl;

		switch (priority)
		{
		case TaskPriority::High:
			return TaskAsync(&highImpl);
		case TaskPriority::Normal:
			return TaskAsync(&normalImpl);
		default:
			ASSERT_CRASH(1 == 0);

			return TaskAsync(&highImpl);
		}
	}

private:
	class TaskAsyncHighPriortiyImpl : public TaskAsyncImpl
	{
	public:
		virtual void Submit() override;
	};

	class TaskAsyncNormalPriortiyImpl : public TaskAsyncImpl
	{
	public:
		virtual void Submit() override;
	};

public:
	void Push(TaskPtr&& task);

	template<TaskPriority priority>
	static TaskAsync* Get()
	{
		static TaskAsync asyncManager = Create(priority);

		return &asyncManager;
	}
};

static void DoAsync(std::function<void()> fIn)
{
	static TaskAsync* asyncManager = TaskAsync::Get<TaskPriority::Normal>();
	asyncManager->Push(MakeTask(std::move(fIn)));
}

template<typename F, typename ...Args>
static void DoAsync(F&& fIn, Args&&... argsIn)
{
	static TaskAsync* asyncManager = TaskAsync::Get<TaskPriority::Normal>();
	asyncManager->Push(MakeTask(std::forward<F>(fIn), std::forward<Args>(argsIn)...));
}

template<typename R, typename T, typename ...Args>
static void DoAsync(R(T::* mem_func)(Args...), shared_ptr<T> obj, Args... args)
{
	static TaskAsync* asyncManager = TaskAsync::Get<TaskPriority::Normal>();
	asyncManager->Push(MakeTask(mem_func, obj, std::forward<Args>(args)...));
}

static void DoAsyncHighPriority(std::function<void()> fIn)
{
	static TaskAsync* asyncManager = TaskAsync::Get<TaskPriority::High>();
	asyncManager->Push(MakeTask(std::move(fIn)));;
}

template<typename F, typename ...Args>
static void DoAsyncHighPriority(F&& fIn, Args&&... argsIn)
{
	static TaskAsync* asyncManager = TaskAsync::Get<TaskPriority::High>();
	asyncManager->Push(MakeTask(std::forward<F>(fIn), std::forward<Args>(argsIn)...));
}

template<typename R, typename T, typename ...Args>
static void DoAsyncHighPriority(R(T::* mem_func)(Args...), shared_ptr<T> obj, Args... args)
{
	static TaskAsync* asyncManager = TaskAsync::Get<TaskPriority::High>();
	asyncManager->Push(MakeTask(mem_func, obj, std::forward<Args>(args)...));
}
