#include "pch.h"
#include "IoService.h"

IoService::IoService(int threadNum)
	:
	_threadNum(threadNum)
{
}

IoService::~IoService()
{
	stop();
}

void IoService::start()
{
	for (int i = 0; i < _threadNum; ++i)
	{
		_ioWorkers.emplace_back([this]() { dispatch(); });
	}
}

void IoService::run()
{
	while (!isFinished())
	{
		this_thread::sleep_for(5s);
	}

	LOG_INFO("finished");
}

void IoService::stop()
{
	dispose();

	for (auto& worker : _ioWorkers)
	{
		if (worker.joinable())
		{
			worker.join();
		}
	}
}