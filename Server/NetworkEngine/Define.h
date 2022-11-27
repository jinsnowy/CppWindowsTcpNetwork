#pragma once

#ifndef _DEFINE_H_
#define _DEFINE_H_

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef uint64 SessionID;

using CondVar = std::condition_variable;
using Thread = std::thread;
using Mutex = std::mutex;
using UniqueLock = std::unique_lock<std::mutex>;
using LockGuard = std::lock_guard<std::mutex>;

using AlarmPtr = std::shared_ptr<class Alarm>;
using BufferSourcePtr = shared_ptr<class BufferSource>;
using SessionPtr = std::shared_ptr<class Session>;
using SessionWeakPtr = std::weak_ptr<class Session>;
using SessionPtrCRef = const SessionPtr&;
using NetworkPtr = std::shared_ptr<class TcpNetwork>;
using NetworkPtrCRef = const std::shared_ptr<class TcpNetwork>&;
using SessionFactory = std::function<SessionPtr()>;
using NetworkFactory = std::function<NetworkPtr(class ServiceBase&)>;
using OnAcceptFunc = std::function<bool(NetworkPtr)>;
using PacketHandlerFunc = std::function<void(SessionPtr)>;
using HandshakePtr = std::unique_ptr<class Handshake>;
using TaskPtr = std::unique_ptr<class Task>;
using PlayerPtr = std::shared_ptr<class Player>;
using ClientSessionFactory = std::function<std::shared_ptr<class ClientSession>()>;
using ServerSessionFactory = std::function<std::shared_ptr<class ServerSession>()>;

static const char* get_bool_str(const bool& v) { return v ? "true" : "false"; }
// check socket call

#define INVALID_SESSION_VALUE 0
#define check_sock_error(exp) ((exp) == SOCKET_ERROR)
#define was_io_pending() ((::WSAGetLastError() == WSA_IO_PENDING))
#define would_block(err) ((::WSAGetLastError() == WSAEWOULDBLOCK))
#define safe_close_event(h) if (h != NULL) { WSACloseEvent(h); h = NULL; }
#define safe_close_handle(h) if (h != NULL) { CloseHandle(h); h = NULL; }
#define get_last_err_msg() WSAGetLastErrorMsg().c_str()
#define get_last_err_msg_code(errCode) WSAGetLastErrorMsg(errCode).c_str()
#define size16(val)		static_cast<int16>(sizeof(val))
#define size32(val)		static_cast<int32>(sizeof(val))

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

template<typename T>
static shared_ptr<T> SharedGlobal()
{
	static shared_ptr<T> inst = make_shared<T>();
	return inst;
}

#endif // !_DEFINE_H_
