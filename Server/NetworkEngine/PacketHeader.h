#pragma once

struct PacketHeader
{
	int protocol = 0;
	int size = 0;

	PacketHeader() = default;

	PacketHeader(int _protocol) : protocol(_protocol), size(0) {}
	PacketHeader(int _protocol, int _size) : protocol(_protocol), size(_size) {}
	PacketHeader(const PacketHeader& rhs) : protocol(rhs.protocol), size(rhs.size) {}
	PacketHeader& operator=(const PacketHeader& rhs) { protocol = rhs.protocol; size = rhs.size; return *this; }

	static PacketHeader Peek(void* buf)
	{
		return *reinterpret_cast<PacketHeader*>(buf);
	}
};