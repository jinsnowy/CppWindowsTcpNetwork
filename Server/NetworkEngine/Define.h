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

static const char* get_bool_str(const bool& v) { return v ? "true" : "false"; }
// check socket call

#define check_sock_error(exp) ((exp) == SOCKET_ERROR)
#define was_io_pending() ((::WSAGetLastError() == WSA_IO_PENDING))
#define would_block(err) ((::WSAGetLastError() == WSAEWOULDBLOCK))
#define safe_close_event(h) if (h != NULL) { WSACloseEvent(h); h = NULL; }
#define safe_close_handle(h) if (h != NULL) { CloseHandle(h); h = NULL; }
#define get_last_err_msg() WSAGetLastErrorMsg().c_str()
#define get_last_err_msg_code(errCode) WSAGetLastErrorMsg(errCode).c_str()

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
#endif // !_DEFINE_H_
