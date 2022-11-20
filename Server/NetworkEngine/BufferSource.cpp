#include "pch.h"
#include "BufferSource.h"
#include "PacketHeader.h"

BufferSegment BufferSource::SerializePacketHeader(int32 protocol, int32 pktLen)
{
	int32 len = pktLen + sizeof(PacketHeader);

	auto buffer = GetBuffer(len);

	CHAR* buf = buffer->lend(len);

	PacketHeader header = PacketHeader(protocol, len);

	memcpy(buf, &header, sizeof(PacketHeader));

	return BufferSegment(buf, len, buffer);
}

BufferSourcePtr BufferSource::GetBuffer(int32 len)
{
	ASSERT_CRASH(len <= BUFFER_SIZE);

	thread_local queue<BufferSourcePtr> bufferQue;

	if (bufferQue.empty())
	{
		bufferQue.push(make());
	}

	auto buffer = bufferQue.front();
	if (buffer->enough(len) == false)
	{
		bufferQue.pop(); bufferQue.push(make());
		buffer = bufferQue.front();
	}

	return buffer;
}
