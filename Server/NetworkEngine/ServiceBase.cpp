#include "pch.h"
#include "ServiceBase.h"
#include "IoContext.h"
#include "TaskScheduler.h"

ServiceBase::ServiceBase(int threadNum)
	:
	_threadNum(threadNum)
{
}

ServiceBase::~ServiceBase()
{
	stop();
}

void ServiceBase::Start()
{
	for (int i = 0; i < _threadNum; ++i)
	{
		_threadWorkers.emplace_back([this]()
		{ 
				ProcessCore();
		});
	}
}

void ServiceBase::Run(std::function<void()> func)
{
	func();
}

void ServiceBase::stop()
{
	_ioContext.Dispose();

	for (auto& worker : _threadWorkers)
	{
		if (worker.joinable())
		{
			worker.join();
		}
	}
}

void ServiceBase::ProcessCore()
{
	TaskScheduler* taskScheduler = TaskScheduler::GetInstance();
	
	int timeSliceMs = 128;

	while (true)
	{
		_ioContext.Dispatch(16);

		taskScheduler->Poll(timeSliceMs);
	}
}
