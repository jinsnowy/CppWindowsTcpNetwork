#pragma once

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

	bool enough(int32 len) { return _capacity >= len; }

	int32 _cursor;
	int32 _capacity;
	CHAR  _buffer[BUFFER_SIZE];

public:
	BufferSource() { ZeroMemory(_buffer, BUFFER_SIZE); _cursor = 0; _capacity = BUFFER_SIZE; }

	static BufferSegment Sink(CHAR* data, int32 len)
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

		CHAR* buf = buffer->copy(data, len);

		return BufferSegment(buf, (ULONG)len, buffer);
	}
};

