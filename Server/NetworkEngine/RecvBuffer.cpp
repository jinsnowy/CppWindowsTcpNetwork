#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer()
{
	ZeroMemory(_buffer, sizeof(_buffer));
	_readPos = 0;
	_writePos = 0;
}

void RecvBuffer::clear()
{
	_readPos = _writePos = 0;
}

void RecvBuffer::rotate()
{
	if (_writePos == _readPos)
	{
		_writePos = _readPos = 0;
		return;
	}

	int32 dataSize = _writePos - _readPos;
	memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
	_readPos = 0;
	_writePos = dataSize;
}

bool RecvBuffer::onDataRecv(int32 recvBytes)
{
	if (isFree(recvBytes) == false)
		return false;

	_writePos += recvBytes;
	return true;
}

void RecvBuffer::read(int32 readBytes)
{
	_readPos += readBytes;
}