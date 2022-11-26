#include "pch.h"
#include "ClientService.h"
#include "ClientSession.h"
#include "Handshake.h"
#include "TcpNetwork.h"

ClientService::ClientService(const ClientServiceParam& param)
	:
	ServiceBase(param.workerNum),
	_clientNum(param.clientNum),
	_endPoint(param.address, param.port),
	_sessionFactory(param.sessionFactory)
{
}

void ClientService::Start()
{
	ServiceBase::Start();

	std::this_thread::sleep_for(2s);

	for (int i = 0; i < _clientNum; ++i)
	{
		auto connector = make_shared<TcpNetwork>(*this);
		auto clientSession = _sessionFactory();
		clientSession->SetConnector(connector);

		auto clientHandshake = new ClientHandshake(connector);
		connector->RequireHandshake(clientHandshake);

		clientSession->ConnectAsync(_endPoint);
		_clients.push_back(clientSession);
	}
}

void ClientService::Broadcast(BufferSegment buffer)
{
	for (auto& client : _clients)
	{
		if (client->IsLogin())
		{
			client->SendAsync(buffer);
		}
	}
}
