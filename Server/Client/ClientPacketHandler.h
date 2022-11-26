#pragma once

#include "UserProtocol.h"

class ClientPacketHandler : public UserPacketHandler
{
protected:
	virtual void onTest(SessionPtrCRef session, UserProtocol::TEST pkt);
	virtual void onLoginResponse(SessionPtrCRef session, UserProtocol::LOGIN_RESPONSE pkt) override;
	virtual void onEnterRoomResponse(SessionPtrCRef session, UserProtocol::ENTER_ROOM_RESPONSE pkt) override;
};
