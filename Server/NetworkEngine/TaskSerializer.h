#pragma once

#include "ITaskCreator.h"

class TaskSchedule;
class TaskSerializer : public ITaskCreator
{
	friend class TaskSchedule;
public:
	TaskSerializer();
	virtual ~TaskSerializer();

public:
	template<typename R, typename T, typename ...Args>
	void Enqueue(R(T::* mem_func)(Args...), Args... args)
	{
		shared_ptr<T> obj = static_pointer_cast<T>(shared_from_this());
		Push(MakeTask(mem_func, obj, std::forward<Args>(args)...));
	}

	size_t Count();

private:
	atomic<bool>     _processing;
	StdMutex	     _mtx;
	queue<TaskPtr>   _tasks;

private:
	void Push(TaskPtr task);

	virtual void Submit() override;
	virtual void EndSchedule() override;
};