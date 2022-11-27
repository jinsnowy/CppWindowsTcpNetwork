#pragma once

#include "protocol/UserProtocolHandler.h"

class ClientPacketHandler : public UserProtocolPacketHandler
{
protected:
	virtual void onTest(SessionPtrCRef session, UserProtocol::TEST pkt);
	virtual void onLoginResponse(SessionPtrCRef session, UserProtocol::LOGIN_RESPONSE pkt) override;
	virtual void onEnterRoomResponse(SessionPtrCRef session, UserProtocol::ENTER_ROOM_RESPONSE pkt) override;
};
