#pragma once
#include "IocpCore.h"
#include "Session.h"

class IoService : protected IocpCore
{
	friend class TcpSocket;
public:
	IoService(int threadWorkerNum);

	~IoService();

	virtual void start();

	virtual void stop();

	virtual void run();

private:
	int _threadNum;
	vector<thread> _ioWorkers;
};