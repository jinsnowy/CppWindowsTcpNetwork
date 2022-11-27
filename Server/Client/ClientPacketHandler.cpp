#include "stdafx.h"
#include "ClientPacketHandler.h"
#include "PlayerSession.h"

void ClientPacketHandler::onTest(SessionPtrCRef session, UserProtocol::TEST pkt)
{
	//LOG_INFO("%s", pkt.text().c_str());
}

void ClientPacketHandler::onLoginResponse(SessionPtrCRef session, UserProtocol::LOGIN_RESPONSE pkt)
{
	session->GetShared<PlayerSession>()->SetPlayer(pkt.player());

	UserProtocol::ENTER_ROOM_REQUEST request;
	session->GetShared<PlayerSession>()->SendAsync(BufferSegment::Serialize(request));
}

void ClientPacketHandler::onEnterRoomResponse(SessionPtrCRef session, UserProtocol::ENTER_ROOM_RESPONSE pkt)
{
	LOG_INFO("Enter Room : %d", pkt.id());
}
