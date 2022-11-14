#pragma once
#include "IoService.h"

struct ClientServiceParam
{
	SessionFactory sessionFactory;
	const char* address;
	uint16 port;
	int clientNum;
	int workerNum;

	ClientServiceParam(SessionFactory _sessionFactory, int _clientNum, int _workerNum, const char* _address, uint16 _port)
		:
		sessionFactory(_sessionFactory), clientNum(_clientNum), workerNum(_workerNum),
		address(_address), port(_port)
	{}
};

class ClientService : public IoService
{
private:
	int _clientNum;
	SessionFactory _sessionFactory;
	EndPoint _endPoint;

public:
	ClientService(const ClientServiceParam& param);

	virtual void start() override;
};