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

static const char* getIoType(IoType type)
{
	switch (type)
	{
	case IO_READ:
		return "IO_READ";
	case IO_WRITE:
		return "IO_WRITE";
	case IO_CONNECT:
		return "IO_CONNECT";
	case IO_DISCONNECT:
		return "IO_DISCONNECT";
	case IO_ACCEPT:
		return "IO_ACCEPT";
	default:
		return "?";
	}
}

struct IoEvent : WSAOVERLAPPED
{
	IoType ioType;

	std::function<void(int32, DWORD)> task;

	template<typename T>
	IoEvent(IoType _ioType, T&& callback)
		:
		WSAOVERLAPPED{}, ioType(_ioType), task(std::forward<T>(callback)) 
	{
	}

	~IoEvent() { /*LOG_INFO("~IoEvent %s", getIoType(ioType)); */ }

	void operator()(int32 errorID, DWORD bytesTransferred) { task(errorID, bytesTransferred); }

	void release() { pool_delete(this); }
};

template<typename T>
IoEvent* makeWriteTask(T&& callback) { return pool_new<IoEvent>(IO_WRITE, std::forward<T>(callback)); }

template<typename T>
IoEvent* makeReadTask(T&& callback) { return pool_new<IoEvent>(IO_READ, std::forward<T>(callback)); }

template<typename T>
IoEvent* makeConnectTask(T&& callback) { return pool_new<IoEvent>(IO_CONNECT, std::forward<T>(callback)); }

template<typename T>
IoEvent* makeDisconnectTask(T&& callback) { return pool_new<IoEvent>(IO_DISCONNECT, std::forward<T>(callback)); }

template<typename T>
IoEvent* makeAcceptTask(T&& callback) { return pool_new<IoEvent>(IO_ACCEPT, std::forward<T>(callback)); }
