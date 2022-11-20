#include "pch.h"
#include "IoService.h"
#include "IoContext.h"

IoService::IoService(int threadNum)
	:
	_threadNum(threadNum)
{
}

IoService::~IoService()
{
	stop();
}

void IoService::Start()
{
	for (int i = 0; i < _threadNum; ++i)
	{
		_ioWorkers.emplace_back([this]() { _ioContext.dispatch(); });
	}
}

void IoService::Run()
{
}

void IoService::stop()
{
	_ioContext.dispose();

	for (auto& worker : _ioWorkers)
	{
		if (worker.joinable())
		{
			worker.join();
		}
	}
}