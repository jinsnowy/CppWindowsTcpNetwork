#include "pch.h"
#include "LoginSession.h"
#include "LoginSessionManager.h"
#include "Handshake.h"
#include "TcpNetwork.h"

LoginSession::LoginSession()
	:
	_createTime(time(NULL))
{
	makeSessionId();
}

void LoginSession::makeSessionId()
{
	static atomic<uint64> sessionId(1000);

	SetSessionId(sessionId.fetch_add(1));
}

void LoginSession::onConnected()
{
	LOG_INFO("LoginSession (%lld) connected", GetSessionId());

	if (_network)
	{
		auto handshake = new ServerHandshake(_network);
		_network->RequireHandshake(handshake);
		_network->ProcessHandshake();
	}
}

void LoginSession::onDisconnected()
{
	LOG_INFO("LoginSession (%lld) disconnected", GetSessionId());

	LoginSessionManager::GetInstance()->removeSession(GetShared<LoginSession>());
}
