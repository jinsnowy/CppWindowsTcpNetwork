#pragma once
#include "Session.h"
#include "IoContext.h"

class IoContext;
class ServiceBase
{
public:
	ServiceBase(int threadWorkerNum);

	~ServiceBase();

	virtual void Start();

	void Run(function<void()> func);

	IoContext& GetContext() { return _ioContext; }

protected:
	virtual void stop();

private:
	IoContext	   _ioContext;
	int			   _threadNum;
	vector<thread> _threadWorkers;

private:
	void ProcessCore();
};