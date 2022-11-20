#include "pch.h"
#include "ClientService.h"
#include "ClientSession.h"
#include "Handshake.h"
#include "TcpNetwork.h"

ClientService::ClientService(const ClientServiceParam& param)
	:
	IoService(param.workerNum),
	_clientNum(param.clientNum),
	_endPoint(param.address, param.port)
{
}

void ClientService::Start()
{
	IoService::Start();

	std::this_thread::sleep_for(2s);

	for (int i = 0; i < _clientNum; ++i)
	{
		auto connector = make_shared<TcpNetwork>(*this);
		auto clientSession = make_shared<ClientSession>();
		clientSession->SetConnector(connector);

		auto clientHandshake = new ClientHandshake(connector);
		connector->RequireHandshake(clientHandshake);

		clientSession->ConnectAsync(_endPoint);
		_clients.push_back(clientSession);
	}
}

void ClientService::Run()
{
	while (1)
	{
		std::this_thread::sleep_for(5s);
	}
}
