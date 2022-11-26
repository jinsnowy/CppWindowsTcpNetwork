#include "pch.h"
#include "TaskAsync.h"
#include "TaskSchedule.h"
#include "TaskScheduler.h"

TaskAsync::TaskAsyncImpl::TaskAsyncImpl()
	:
	scheduler(TaskScheduler::GetInstance())
{
}

TaskAsync::TaskAsyncImpl::~TaskAsyncImpl()
{
}

void TaskAsync::TaskAsyncImpl::Push(TaskPtr&& task)
{
	{
		StdWriteLock lk(_mtx);
		_tasks.push(std::move(task));
	}
	
	Submit();
}

void TaskAsync::TaskAsyncNormalPriortiyImpl::Submit()
{
	TaskSchedule* schedule;

	{
		StdWriteLock lk(_mtx);
		if (_tasks.empty()) {
			return;
		}

		schedule = new TaskSchedule();
		schedule->tasks = std::move(_tasks);
	}

	scheduler->Schedule(schedule);
}

void TaskAsync::TaskAsyncHighPriortiyImpl::Submit()
{
	TaskSchedule* schedule;

	{
		StdWriteLock lk(_mtx);
		if (_tasks.empty()) {
			return;
		}

		schedule = new TaskSchedule();
		schedule->tasks = std::move(_tasks);
	}

	scheduler->HighPrioritySchedule(schedule);
}

TaskAsync::TaskAsync(TaskAsyncImpl* impl)
	:
	_impl(impl)
{
}

void TaskAsync::Push(TaskPtr&& task)
{
	_impl->Push(std::move(task));
}
