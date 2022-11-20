#include "pch.h"
#include "ServerSession.h"

ServerSession::ServerSession()
	:
	Session()
{
}

void ServerSession::onConnected()
{
	Session::onConnected();

	LOG_INFO("ServerSession (%lld) connected", GetSessionId());
}

void ServerSession::onDisconnected()
{
	Session::onDisconnected();

	LOG_INFO("LoginSession (%lld) disconnected", GetSessionId());
}
