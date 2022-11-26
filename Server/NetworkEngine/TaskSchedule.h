#pragma once

#include "ITaskCreator.h"

class TaskSchedule : public PoolObject
{
	friend class TaskScheduler;
public:
	std::queue<TaskPtr> tasks;
	std::shared_ptr<ITaskCreator> taskCreator;

public:
	TaskSchedule();
	TaskSchedule(shared_ptr<ITaskCreator> _creator);
	TaskSchedule(shared_ptr<ITaskCreator> _creator, queue<TaskPtr> _tasks);
	
	~TaskSchedule();

	void Flush(int& timeOutMs);

private:
	void Done();
};
