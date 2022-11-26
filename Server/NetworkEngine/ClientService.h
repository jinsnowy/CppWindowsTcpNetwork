#pragma once
#include "ServiceBase.h"
#include "ClientSession.h"

struct ClientServiceParam
{
	const char* address;
	uint16 port;
	int clientNum;
	int workerNum;
	ClientSessionFactory sessionFactory;

	ClientServiceParam(int _clientNum, int _workerNum, const char* _address, uint16 _port, ClientSessionFactory _sessionFactory)
		:
		clientNum(_clientNum), workerNum(_workerNum),
		address(_address), port(_port), sessionFactory(_sessionFactory)
	{}
};

class ClientSession;
class ClientService : public ServiceBase
{
private:
	int _clientNum;
	EndPoint _endPoint;
	ClientSessionFactory _sessionFactory;
	vector<shared_ptr<ClientSession>> _clients;

public:
	ClientService(const ClientServiceParam& param);

	virtual void Start() override;

	void Broadcast(BufferSegment buffer);
};