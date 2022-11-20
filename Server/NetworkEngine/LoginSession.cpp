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
	std::string loginId;

	auto t = _createTime;

	while (t > 0)
	{
		char c = (t & 0xFF) % 128;

		loginId.push_back(c);

		t >>= 16;
	}

	static random_device rd;
	static default_random_engine rng(rd());
	
	std::shuffle(loginId.begin(), loginId.end(), rng);

	SetSessionId(std::hash<std::string>{}(loginId));
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

	LoginSessionManager::getInstance()->removeSession(GetShared<LoginSession>());
}
