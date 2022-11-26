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
}

void ServerSession::onDisconnected()
{
	Session::onDisconnected();
}
