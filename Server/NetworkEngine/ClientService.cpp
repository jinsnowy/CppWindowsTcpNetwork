#include "pch.h"
#include "ClientService.h"

ClientService::ClientService(const ClientServiceParam& param)
	:
	IoService(param.workerNum),
	_clientNum(param.clientNum),
	_sessionFactory(param.sessionFactory),
	_endPoint(param.address, param.port)
{
}

void ClientService::start()
{
	IoService::start();

	std::this_thread::sleep_for(2s);

	for (int i = 0; i < _clientNum; ++i)
	{
		auto sessionPtr = _sessionFactory(*this);
		sessionPtr->connectAsync(_endPoint);
	}
}
