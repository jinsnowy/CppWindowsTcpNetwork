#pragma once
#include "Protocol.h"

class BufferSource;
using BufferSourcePtr = shared_ptr<BufferSource>;
struct BufferSegment : WSABUF
{
	BufferSourcePtr source;

	WSABUF wsaBuf() { return WSABUF{len, buf}; }

	BufferSegment(CHAR* _buf, ULONG _len, const BufferSourcePtr& _ptr) : WSABUF{ _len, _buf }, source(_ptr) {}
};

class BufferSource
{
	enum 
	{
		BUFFER_SIZE = 4096,
	};

private:
	static BufferSourcePtr make() { return make_shared<BufferSource>(); }

	CHAR* copy(CHAR* buf, int32 len)
	{
		CHAR* front = &_buffer[_cursor];
		memcpy(front, buf, len);

		_capacity -= len;
		_cursor += len;
		ASSERT_CRASH(_capacity >= 0);

		return front;
	}

	CHAR* lend(int32 len)
	{
		CHAR* front = &_buffer[_cursor];

		_capacity -= len;
		_cursor += len;
		ASSERT_CRASH(_capacity >= 0);

		return front;
	}

	bool enough(int32 len) { return _capacity >= len; }

	int32 _cursor;
	int32 _capacity;
	CHAR  _buffer[BUFFER_SIZE];

public:
	BufferSource() { ZeroMemory(_buffer, BUFFER_SIZE); _cursor = 0; _capacity = BUFFER_SIZE; }
	
	template<typename T>
	static BufferSegment DefaultSerialize(T& packet)
	{
		packet.size = sizeof(T);
		return Sink((CHAR*)&packet, sizeof(T));
	}

	template<typename T>
	static BufferSegment Sink(const T& pkt)
	{
		int32 pktLen = (int32)pkt.ByteSizeLong();
		auto segment = SerializePacketHeader(Protocol::GetProtocol<T>(), pktLen);

		if (!pkt.SerializeToArray(segment.buf + sizeof(PacketHeader), pktLen))
		{
			return BufferSegment(NULL, 0, NULL);
		}

		return segment;
	}

	static BufferSegment Sink(CHAR* data, int32 len)
	{
		auto buffer = GetBuffer(len);

		CHAR* buf = buffer->copy(data, len);

		return BufferSegment(buf, (ULONG)len, buffer);
	}
private:
	static BufferSegment SerializePacketHeader(int32 protocol, int32 pktLen);

	static BufferSourcePtr GetBuffer(int32 len);
};

