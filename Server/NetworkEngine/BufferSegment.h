#pragma once
#include "PacketHeader.h"
#include "Protocol.h"

struct BufferSegment : WSABUF
{
	BufferSourcePtr source;

	WSABUF wsaBuf() { return WSABUF{ len, buf }; }

	BufferSegment(CHAR* _buf, ULONG _len, const BufferSourcePtr& _ptr) : WSABUF{ _len, _buf }, source(_ptr) {}

	template<typename T>
	static BufferSegment Serialize(const T& pkt)
	{
		int pktLen = (int)pkt.ByteSizeLong();
		auto segment = BufferSource::SerializePacketHeader(Protocol::GetProtocol<T>(), pktLen);

		if (!pkt.SerializeToArray(segment.buf + sizeof(PacketHeader), pktLen))
		{
			return BufferSegment(NULL, 0, NULL);
		}

		return segment;
	}
};
