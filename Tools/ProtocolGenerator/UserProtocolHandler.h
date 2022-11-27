#pragma once
#include "UserProtocol.pb.h"

enum USERPROTOCOL; : int
{
	PKT_HEARTBEAT = 1000,
	PKT_TEST = 1001,
	PKT_LOGIN_REQUEST = 1002,
	PKT_LOGIN_RESPONSE = 1003,
	PKT_ENTER_ROOM_REQUEST = 1004,
	PKT_ENTER_ROOM_RESPONSE = 1005,

	USERPROTOCOL_START = 1000,
	USERPROTOCOL_END = 1005
};
USE_PROTOCOL(UserProtocol::HEARTBEAT);
USE_PROTOCOL(UserProtocol::TEST);
USE_PROTOCOL(UserProtocol::LOGIN_REQUEST);
USE_PROTOCOL(UserProtocol::LOGIN_RESPONSE);
USE_PROTOCOL(UserProtocol::ENTER_ROOM_REQUEST);
USE_PROTOCOL(UserProtocol::ENTER_ROOM_RESPONSE);

class UserProtocolPacketHandler : public PacketHandler
{
public:
	virtual bool IsValidProtocol(int32 protocol) override
	{
		return protocol >= USERPROTOCOL_START && protocol <= USERPROTOCOL_END;
	}

	virtual void HandleRecv(SessionPtrCRef session, const PacketHeader& header, CHAR* buffer) override
	{
		switch (header.protocol)
		{
		case PKT_HEARTBEAT:
			onHeartbeat(session, parse<UserProtocol::HEARTBEAT)>(buffer, header.size));
			break;
		case PKT_TEST:
			onTest(session, parse<UserProtocol::TEST)>(buffer, header.size));
			break;
		case PKT_LOGIN_REQUEST:
			onLoginRequest(session, parse<UserProtocol::LOGIN_REQUEST)>(buffer, header.size));
			break;
		case PKT_LOGIN_RESPONSE:
			onLoginResponse(session, parse<UserProtocol::LOGIN_RESPONSE)>(buffer, header.size));
			break;
		case PKT_ENTER_ROOM_REQUEST:
			onEnterRoomRequest(session, parse<UserProtocol::ENTER_ROOM_REQUEST)>(buffer, header.size));
			break;
		case PKT_ENTER_ROOM_RESPONSE:
			onEnterRoomResponse(session, parse<UserProtocol::ENTER_ROOM_RESPONSE)>(buffer, header.size));
			break;
		}
	}

public:
	virtual void onHeartbeat(SessionPtrCRef, UserProtocol::HEARTBEAT pkt) {}
	virtual void onTest(SessionPtrCRef, UserProtocol::TEST pkt) {}
	virtual void onLoginRequest(SessionPtrCRef, UserProtocol::LOGIN_REQUEST pkt) {}
	virtual void onLoginResponse(SessionPtrCRef, UserProtocol::LOGIN_RESPONSE pkt) {}
	virtual void onEnterRoomRequest(SessionPtrCRef, UserProtocol::ENTER_ROOM_REQUEST pkt) {}
	virtual void onEnterRoomResponse(SessionPtrCRef, UserProtocol::ENTER_ROOM_RESPONSE pkt) {}
};