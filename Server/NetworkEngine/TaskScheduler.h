#pragma once
#include "Define.h"

class TaskSchedule;
class TaskScheduler : public ISingleton<TaskScheduler>
{
public:
	TaskScheduler();
	~TaskScheduler();

	void Poll(int timeOutMs);

	void Schedule(TaskSchedule* schedule);

	void ReSchedule(TaskSchedule* _schedule);

	void HighPrioritySchedule(TaskSchedule* _schedule);

	bool IsIdle();

	size_t GlobalTaskQueueSize();

private:
	StdMutex _mtx;
	atomic<int> _processor_count;
	deque<TaskSchedule*> _global_que;
};