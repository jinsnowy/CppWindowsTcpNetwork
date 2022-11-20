#pragma once

class RecvBuffer
{
	enum
	{
		BUFFER_SIZE = 4096
	};
public:
	RecvBuffer();

	CHAR* getBufferPtr() { return &_buffer[_writePos]; }
	int32 getLen() { return BUFFER_SIZE - _writePos; }

	CHAR* getBufferPtrRead() { return &_buffer[_readPos]; }
	
	void clear();
	void rotate();
	bool onDataRecv(int32 recvBytes);
	void read(int32 readBytes);
	bool isReadable(int32 bytes) { return _writePos - _readPos >= bytes; }
	bool isHeaderReadable() { return _writePos - _readPos >= sizeof(PacketHeader); }
private:
	bool isFree(int32 bytes) { return BUFFER_SIZE - _writePos >= bytes; }

private:
	int32 _writePos;
	int32 _readPos;
	CHAR  _buffer[BUFFER_SIZE];
};

