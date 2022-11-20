#pragma once
#include "Session.h"
#include "IoContext.h"

class IoContext;
class IoService
{
public:
	IoService(int threadWorkerNum);

	~IoService();

	virtual void Start();

	virtual void Run();

	IoContext& GetContext() { return _ioContext; }

protected:
	virtual void stop();

private:
	IoContext	   _ioContext;
	int			   _threadNum;
	vector<thread> _ioWorkers;
};