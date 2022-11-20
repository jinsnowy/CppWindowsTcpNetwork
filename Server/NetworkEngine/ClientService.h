#pragma once
#include "IoService.h"

struct ClientServiceParam
{
	const char* address;
	uint16 port;
	int clientNum;
	int workerNum;

	ClientServiceParam(int _clientNum, int _workerNum, const char* _address, uint16 _port)
		:
		clientNum(_clientNum), workerNum(_workerNum),
		address(_address), port(_port)
	{}
};

class ClientSession;
class ClientService : public IoService
{
private:
	int _clientNum;
	EndPoint _endPoint;
	vector<shared_ptr<ClientSession>> _clients;

public:
	ClientService(const ClientServiceParam& param);

	virtual void Start() override;

	virtual void Run() override;
};