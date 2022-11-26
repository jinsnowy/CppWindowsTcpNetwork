#include "pch.h"
#include "TaskScheduler.h"
#include "TaskSchedule.h"

TaskScheduler::TaskScheduler()
	:
	_processor_count(0)
{
}

TaskScheduler::~TaskScheduler()
{
}

void TaskScheduler::Poll(int timeOutMs)
{
	TaskSchedule* schedule;
	while (timeOutMs > 0) 
	{
		{
			StdWriteLock lk(_mtx);
			if (_global_que.empty()) {
				return;
			}

			schedule = _global_que.front();
			_global_que.pop_front();

			_processor_count.fetch_add(1);
		}

		schedule->Flush(OUT timeOutMs);

		_processor_count.fetch_sub(1);
	}
}

void TaskScheduler::Schedule(TaskSchedule* schedule)
{
	StdWriteLock lk(_mtx);
	_global_que.push_back(schedule);
}

void TaskScheduler::ReSchedule(TaskSchedule* _schedule)
{
	{
		StdWriteLock lk(_mtx);
		_global_que.push_front(_schedule);
	}

	// LOG_INFO("ReScheduled %llu", _schedule->tasks.size());
}

void TaskScheduler::HighPrioritySchedule(TaskSchedule* _schedule)
{
	StdWriteLock lk(_mtx);
	_global_que.push_front(_schedule);
}

bool TaskScheduler::IsIdle()
{
	StdWriteLock lk(_mtx);
	return _global_que.size() == 0 && _processor_count == 0;
}

size_t TaskScheduler::GlobalTaskQueueSize()
{
	StdWriteLock lk(_mtx);
	return _global_que.size();
}
