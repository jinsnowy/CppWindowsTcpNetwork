#include "pch.h"
#include "TaskSerializer.h"
#include "TaskScheduler.h"
#include "TaskSchedule.h"

TaskSerializer::TaskSerializer()
	:
	_processing(false)
{
}

TaskSerializer::~TaskSerializer()
{
}

size_t TaskSerializer::Count()
{
	StdWriteLock lk(_mtx);
	return _tasks.size();
}

void TaskSerializer::Push(TaskPtr task)
{
	{
		StdWriteLock lk(_mtx);
		_tasks.push(std::move(task));
	}

	if (_processing.exchange(true) == false)
	{
		Submit();
	}
}

void TaskSerializer::Submit()
{
	static TaskScheduler* taskScheduler = TaskScheduler::GetInstance();

	int count;

	TaskSchedule* schedule;

	{
		StdWriteLock lk(_mtx);
		if (_tasks.empty()) {
			_processing.store(false);
			return;
		}

		count = (int)_tasks.size();

		schedule = new TaskSchedule();
		schedule->tasks = std::move(_tasks);
	}

	LOG_INFO("submit %d tasks", count);

	schedule->taskCreator = shared_from_this();

	taskScheduler->Schedule(schedule);
}

void TaskSerializer::EndSchedule()
{
	Submit();
}