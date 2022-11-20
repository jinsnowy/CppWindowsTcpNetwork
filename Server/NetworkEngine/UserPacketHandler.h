#pragma once
#include "UserProtocol.h"

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
			onHeartBeat(session, parse<UserProtocol::HEARTBEAT>(buffer, header.size));
			break;
		case TEST:
			onTestMessage(session, parse<UserProtocol::TEST>(buffer, header.size));
			break;
		}
	}

private:
	static void onHeartBeat(SessionPtrCRef session, UserProtocol::HEARTBEAT pkt);
	static void onTestMessage(SessionPtrCRef session, UserProtocol::TEST pkt);
}; 

