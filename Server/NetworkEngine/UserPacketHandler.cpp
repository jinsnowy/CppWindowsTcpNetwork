#include "pch.h"
#include "UserPacketHandler.h"
#include "Session.h"

void UserPacketHandler::onHeartBeat(SessionPtrCRef session, UserProtocol::HEARTBEAT pkt)
{
}

void UserPacketHandler::onTestMessage(SessionPtrCRef session, UserProtocol::TEST pkt)
{
	LOG_INFO("Session(%llu) : message %s", session->GetSessionId(), pkt.text().c_str());
}
