#include "stdafx.h"
#include "UserSession.h"
#include "TcpNetwork.h"
#include "ServerPacketHandler.h"

UserSession::UserSession()
{
}

void UserSession::onConnected()
{
	ServerSession::onConnected();

	GetNetwork()->InstallPacketHandler<ServerPacketHandler>();
}

void UserSession::onAuthorized()
{
	ServerSession::onAuthorized();
}

void UserSession::SetPlayer(UserProtocol::Player player)
{
	_player = make_shared<Player>(*this);
	_player->playerId = player.id();
	_player->name = player.name();
}
