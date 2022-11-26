#pragma once
#include "UserProtocol.h"

class ServerPacketHandler : public UserPacketHandler
{
public:
	virtual void onHeartbeat(SessionPtrCRef session, UserProtocol::HEARTBEAT pkt) override;
	virtual void onTest(SessionPtrCRef session, UserProtocol::TEST pkt) override;
	virtual void onLoginRequest(SessionPtrCRef session, UserProtocol::LOGIN_REQUEST pkt) override;
	virtual void onEnterRoomRequest(SessionPtrCRef session, UserProtocol::ENTER_ROOM_REQUEST pkt) override;
}; 

