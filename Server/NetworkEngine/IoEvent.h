#pragma once

#include "MemoryPool.h"

enum IoType
{
	IO_READ,
	IO_WRITE,
	IO_CONNECT,
	IO_DISCONNECT,
	IO_ACCEPT,
};

struct IoEvent : WSAOVERLAPPED
{
	IoType ioType;

	IoEvent(IoType _ioType) : WSAOVERLAPPED{}, ioType(_ioType) { }

	~IoEvent() { LOG_INFO("~IoEvent"); }

	virtual void operator()(int32 errorID, DWORD bytesTransferred) {}

	void release() { pool_delete(this); }
};

template<typename T>
struct IoEventWithTask : IoEvent
{
	T callback;

	IoEventWithTask(IoType _ioType, T&& callback)
		:
		IoEvent(_ioType),
		callback(std::forward<T>(callback))
	{
	}

	~IoEventWithTask()
	{
		LOG_INFO("~IoEventWithTask");
	}

	virtual void operator()(int32 errorId, DWORD bytesTransferred)
	{
		callback(errorId, bytesTransferred);
	}
};

template<typename T>
IoEvent* makeWriteTask(T&& callback) { return pool_new<IoEventWithTask<T>>(IO_WRITE, std::forward<T>(callback)); }

template<typename T>
IoEvent* makeReadTask(T&& callback) { return pool_new<IoEventWithTask<T>>(IO_READ, std::forward<T>(callback)); }

template<typename T>
IoEvent* makeConnectTask(T&& callback) { return pool_new<IoEventWithTask<T>>(IO_CONNECT, std::forward<T>(callback)); }

template<typename T>
IoEvent* makeDisconnectTask(T&& callback) { return pool_new<IoEventWithTask<T>>(IO_DISCONNECT, std::forward<T>(callback)); }

template<typename T>
IoEvent* makeAcceptTask(T&& callback) { return pool_new<IoEventWithTask<T>>(IO_ACCEPT, std::forward<T>(callback)); }
