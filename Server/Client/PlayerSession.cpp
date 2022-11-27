#include "stdafx.h"
#include "PlayerSession.h"
#include "TcpNetwork.h"
#include "ClientPacketHandler.h"

void PlayerSession::onConnected()
{
	ClientSession::onConnected();

	GetNetwork()->InstallPacketHandler<ClientPacketHandler>();
}

void PlayerSession::onAuthorized()
{
	ClientSession::onAuthorized();

	static atomic<uint64> _playerId(0);

	uint64 playerId = _playerId.fetch_add(1);
	UserProtocol::LOGIN_REQUEST request;
	request.mutable_player()->set_id(playerId);
	request.mutable_player()->set_name("player_" + to_string(playerId));

	SendAsync(BufferSegment::Serialize(request));
}
