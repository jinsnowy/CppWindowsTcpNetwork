#pragma once

#include "protocol\UserProtocol.pb.h"

enum USER_PROTOCOL : int
{
	HEATBEAT = 0,
	TEST = 1,
	LOGIN_REQ,
	LOGIN_RES,
	ENTER_ROOM_REQ,
	ENTER_ROOM_RES,
	USER_PROTOCOL_START = HEATBEAT,
	USER_PROTOCOL_END = ENTER_ROOM_RES,
};

USE_PROTOCOL(UserProtocol::TEST);
USE_PROTOCOL(UserProtocol::HEARTBEAT);
USE_PROTOCOL(UserProtocol::LOGIN_REQUEST);
USE_PROTOCOL(UserProtocol::LOGIN_RESPONSE);
USE_PROTOCOL(UserProtocol::ENTER_ROOM_REQUEST);
USE_PROTOCOL(UserProtocol::ENTER_ROOM_RESPONSE);

class UserPacketHandler : public PacketHandler
{
public:
	virtual bool IsValidProtocol(int32 protocol) override
	{
		return protocol >= USER_PROTOCOL_START && protocol <= USER_PROTOCOL_END;
	}

	virtual void HandleRecv(SessionPtrCRef session, const PacketHeader& header, CHAR* buffer) override
	{
		switch (header.protocol)
		{
		case HEATBEAT:
			onHeartbeat(session, parse<UserProtocol::HEARTBEAT>(buffer, header.size));
			break;
		case TEST:
			onTest(session, parse<UserProtocol::TEST>(buffer, header.size));
			break;
		case LOGIN_REQ:
			onLoginRequest(session, parse<UserProtocol::LOGIN_REQUEST>(buffer, header.size));
			break;
		case LOGIN_RES:
			onLoginResponse(session, parse<UserProtocol::LOGIN_RESPONSE>(buffer, header.size));
			break;
		case ENTER_ROOM_REQ:
			onEnterRoomRequest(session, parse<UserProtocol::ENTER_ROOM_REQUEST>(buffer, header.size));
			break;
		case ENTER_ROOM_RES:
			onEnterRoomResponse(session, parse<UserProtocol::ENTER_ROOM_RESPONSE>(buffer, header.size));
			break;
		}
	}

public:
	virtual void onTest(SessionPtrCRef session, UserProtocol::TEST pkt) {}
	virtual void onHeartbeat(SessionPtrCRef session, UserProtocol::HEARTBEAT pkt) {}
	virtual void onLoginRequest(SessionPtrCRef session, UserProtocol::LOGIN_REQUEST pkt) {}
	virtual void onLoginResponse(SessionPtrCRef session, UserProtocol::LOGIN_RESPONSE pkt) {}
	virtual void onEnterRoomRequest(SessionPtrCRef session, UserProtocol::ENTER_ROOM_REQUEST pkt) {}
	virtual void onEnterRoomResponse(SessionPtrCRef session, UserProtocol::ENTER_ROOM_RESPONSE pkt) {}
};