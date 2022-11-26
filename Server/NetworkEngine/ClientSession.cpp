#include "pch.h"
#include "ClientSession.h"
#include "TcpNetwork.h"

ClientSession::ClientSession()
	:
	Session(),
	_logined(false)
{
}

void ClientSession::SetConnector(shared_ptr<TcpNetwork> connector)
{
	_connector = connector;
	if (_connector)
	{
		_connector->AttachSession(shared_from_this());
	}
}

bool ClientSession::Connect(const char* address, uint16 port)
{
	return Connect(EndPoint(address, port));
}

bool ClientSession::Connect(const EndPoint& endPoint)
{
	if (IsConnected())
	{
		LOG_INFO("already connected");
		return true;
	}

	_connector->ConnectAsync(endPoint);

	int count = 0;
	while (!IsConnected() && ++count < 10)
	{
		std::this_thread::sleep_for(200ms);
	}

	return IsConnected();
}

bool ClientSession::Reconnect(const EndPoint& endPoint)
{
	if (IsConnected())
	{
		Disconnect();
	}

	LOG_INFO("reconnect ...");

	return Connect(endPoint);
}

void ClientSession::ConnectAsync(const EndPoint& endPoint)
{
	if (IsConnected())
	{
		LOG_INFO("already connected");
		return;
	}

	_connector->ConnectAsync(endPoint);
}

void ClientSession::ReconnectAsync(const EndPoint& endPoint)
{
	if (IsConnected())
	{
		DisconnectAsync();
	}

	return ConnectAsync(endPoint);
}

void ClientSession::onConnected()
{
	Session::onConnected();
}

void ClientSession::onDisconnected()
{
	Session::onDisconnected();

	_logined = false;
}

void ClientSession::onAuthorized()
{
	_logined = true;
}
