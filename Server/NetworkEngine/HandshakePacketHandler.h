#pragma once
#include "HandshakeProtocol.h"

class HandshakePacketHandler : public PacketHandler
{
public:
	virtual bool IsValidProtocol(int32 protocol) override
	{
		return protocol >= HANDSHAKE_PROTOCOL_START && protocol <= HANDSHAKE_PROTOCOL_END;
	}

	virtual void HandleRecv(SessionPtrCRef session, const PacketHeader& header, CHAR* buffer) override
	{
		switch (header.protocol)
		{
		case CLIENT_HELLO:
			onClientHello(session, interpret<PKT_CLIENT_HELLO>(buffer));
			break;
		case SERVER_HELLO:
			onServerHello(session, interpret<PKT_SERVER_HELLO>(buffer));
			break;
		}
	}

private:
	template<typename T>
	T* interpret(CHAR* buffer) { return reinterpret_cast<T*>(buffer); }

	static void onClientHello(SessionPtrCRef session, PKT_CLIENT_HELLO* pkt);
	static void onServerHello(SessionPtrCRef session, PKT_SERVER_HELLO* pkt);
};