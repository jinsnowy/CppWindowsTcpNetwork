#include "pch.h"
#include "TaskSchedule.h"
#include "TaskScheduler.h"
#include "TaskSerializer.h"

TaskSchedule::TaskSchedule()
{
}

TaskSchedule::TaskSchedule(shared_ptr<ITaskCreator> _creator)
	:
	taskCreator(_creator)
{
}

TaskSchedule::TaskSchedule(shared_ptr<ITaskCreator> _creator, queue<TaskPtr> _tasks)
	:
	taskCreator(_creator),
	tasks(std::move(_tasks))
{
}

TaskSchedule::~TaskSchedule()
{
}

void TaskSchedule::Flush(int& timeOutMs)
{
	static TaskScheduler* scheduler = TaskScheduler::GetInstance();

	thread_local Timer timer = Timer::startNew();

	// LOG_INFO("Enter %llu tasks", tasks.size());

	while (timeOutMs > 0 && tasks.empty() == false)
	{
		auto task = std::move(tasks.front());
		tasks.pop();

		timer.start();

		(*task)();

		timeOutMs -= timer.count<std::chrono::milliseconds>();
	}

	// LOG_INFO("Leave %llu tasks", tasks.size());
	
	if (tasks.empty() == false)
	{
		scheduler->ReSchedule(this);
	}
	else
	{
		Done();
	}
}

void TaskSchedule::Done()
{
	if (taskCreator) {
		taskCreator->EndSchedule();
	}

	delete this;
}
