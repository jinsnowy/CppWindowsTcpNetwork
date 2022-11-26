#include "pch.h"
#include "Session.h"
#include "SessionManager.h"
#include "TcpNetwork.h"

Session::Session()
	:
	_sessionId(INVALID_SESSION_VALUE)
{
}

Session::~Session()
{
	LOG_INFO("~Session()");
}

void Session::SendAsync(const BufferSegment& segment)
{
	if (IsConnected() == false || segment.len == 0)
		return;

	_network->SendAsync(segment);
}

bool Session::onRecv(const PacketHeader& header, CHAR* dataStartPtr)
{
	return true;
}

void Session::attachNetwork(shared_ptr<TcpNetwork> network)
{
	if (_network != nullptr && _network->IsConnected())
	{
		_network->DisconnectAsync();
		_network = nullptr;
	}

	_network = network;

	if (_network->IsConnected())
	{
		onConnected();
	}
}

void Session::detachNetwork()
{
	if (IsConnected())
	{
		onDisconnected();
	}

	_network = nullptr;
}

void Session::recv(DWORD recvBytes)
{
}

bool Session::IsConnected()
{
	return _network != nullptr && _network->IsConnected();
}

void Session::Disconnect()
{
	if (_network == nullptr || _network->IsConnected() == false)
		return;

	_network->DisconnectAsync();

	int count = 0;
	while (IsConnected() && ++count < 10)
	{
		std::this_thread::sleep_for(100ms);
	}
}

void Session::DisconnectAsync()
{
	if (_network == nullptr || _network->IsConnected() == false)
		return;

	_network->DisconnectAsync();
}

void Session::onAuthorized()
{
}

void Session::onConnected()
{
	LOG_INFO("connected to %s", GetEndPointDesc().c_str());
}

void Session::onDisconnected()
{
	LOG_INFO("disconnected from %s", GetEndPointDesc().c_str());
}

EndPoint Session::GetEndPoint()
{
	return _network != nullptr ? _network->GetEndPoint() : EndPoint();
}
