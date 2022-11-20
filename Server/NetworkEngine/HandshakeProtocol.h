#pragma once

enum HANDSHAKE_PROTOCOL : int
{
	CLIENT_HELLO = -100,
	SERVER_HELLO,
	HANDSHAKE_PROTOCOL_START = CLIENT_HELLO,
	HANDSHAKE_PROTOCOL_END = SERVER_HELLO,
};

struct PKT_CLIENT_HELLO : PacketHeader
{
	SessionID SessionId;
	PKT_CLIENT_HELLO(SessionID _SessionId)
		:
		PacketHeader(CLIENT_HELLO),
		SessionId(_SessionId)
	{}
};

struct PKT_SERVER_HELLO : PacketHeader
{
	SessionID SessionId;
	PKT_SERVER_HELLO()
		:
		PacketHeader(SERVER_HELLO),
		SessionId(0)
	{}
};