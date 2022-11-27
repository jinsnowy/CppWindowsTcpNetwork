#pragma once

class RecvBuffer
{
	enum
	{
		BUFFER_SIZE = 4096
	};
public:
	RecvBuffer();

	CHAR* GetBufferPtr() { return &_buffer[_writePos]; }
	int32 GetLen() { return BUFFER_SIZE - _writePos; }
	CHAR* GetBufferPtrRead() { return &_buffer[_readPos]; }
	
	void Clear();
	void Rotate();
	bool OnDataRecv(int32 recvBytes);
	void Read(int32 readBytes);
	bool IsReadable(int32 bytes) { return _writePos - _readPos >= bytes; }
	bool IsHeaderReadable() { return _writePos - _readPos >= sizeof(PacketHeader); }

private:
	bool isFree(int32 bytes) { return BUFFER_SIZE - _writePos >= bytes; }

private:
	int32 _writePos;
	int32 _readPos;
	CHAR  _buffer[BUFFER_SIZE];
};

