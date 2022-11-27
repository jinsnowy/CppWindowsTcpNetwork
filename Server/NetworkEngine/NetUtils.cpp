#include "pch.h"
#include "NetUtils.h"

LPFN_CONNECTEX NetUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX NetUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX NetUtils::AcceptEx = nullptr;

void NetUtils::initialize()
{
	struct WSAInit
	{
		WSAInit()
		{
			WSADATA wsaData;
			ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);
		}

		~WSAInit()
		{
			::WSACleanup();
		}
	};

	static WSAInit init;

	/* 런타임에 주소 얻어오는 API */
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	Close(dummySocket);
}

bool NetUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;
	bool result = SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
	return result;
}

SOCKET NetUtils::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool NetUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool NetUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool NetUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool NetUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool NetUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

bool NetUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool NetUtils::Bind(SOCKET socket, EndPoint netAddr)
{
	return SOCKET_ERROR != ::bind(socket, netAddr.GetSockAddr(), sizeof(SOCKADDR_IN));
}

bool NetUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
}

bool NetUtils::GetEndPoint(SOCKET socket, EndPoint& endPoint)
{
	int32 sizeOfSockAddress = sizeof(SOCKADDR);
	return SOCKET_ERROR != ::getpeername(socket, endPoint.GetSockAddr(), &sizeOfSockAddress);
}

bool NetUtils::Listen(SOCKET socket, int32 backLog)
{
	return SOCKET_ERROR != ::listen(socket, backLog);
}

void NetUtils::Shutdown(SOCKET socket, int32 how)
{
	if (socket != INVALID_SOCKET)
		::shutdown(socket, how);
}

void NetUtils::Close(SOCKET socket)
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);

	socket = INVALID_SOCKET;
}

bool NetUtils::ConnectAsync(SOCKET socket, LPWSAOVERLAPPED overlapped, const EndPoint& endPoint)
{
	DWORD dwSentBytes = 0;
	if (!ConnectEx(socket, endPoint.GetSockAddr(), sizeof(SOCKADDR), NULL, 0, &dwSentBytes, overlapped))
	{
		int32 errorCode = ::WSAGetLastError();

		if (errorCode != WSA_IO_PENDING)
		{
			string msg = get_last_err_msg();
		}

		return was_io_pending();
	}

	return true;
}

bool NetUtils::DisconnectAsync(SOCKET socket, LPWSAOVERLAPPED overlapped)
{
	if (!DisconnectEx(socket, overlapped, TF_REUSE_SOCKET, 0))
	{
		return was_io_pending();
	}

	return true;
}

bool NetUtils::AcceptAsync(SOCKET listenSocket, SOCKET acceptSocket, LPVOID recvBuf, LPWSAOVERLAPPED overlapped)
{
	DWORD dwRecvBytes = 0;
	if (!AcceptEx(listenSocket, acceptSocket, recvBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwRecvBytes, overlapped))
	{
		return was_io_pending();
	}

	return true;
}

bool NetUtils::WriteAsync(SOCKET socket, LPWSAOVERLAPPED overlapped, CHAR* buf, ULONG len)
{
	DWORD dwSentBytes = 0;
	WSABUF wsaBuf;
	wsaBuf.buf = buf;
	wsaBuf.len = len;

	if (check_sock_error(::WSASend(socket, &wsaBuf, 1, &dwSentBytes, 0, overlapped, NULL)))
	{
		return was_io_pending();
	}

	return true;
}

bool NetUtils::WriteAsync(SOCKET socket, LPWSAOVERLAPPED overlapped, WSABUF* buffers, DWORD bufferLen)
{
	DWORD dwSentBytes = 0;
	if (check_sock_error(::WSASend(socket, buffers, bufferLen, &dwSentBytes, 0, overlapped, NULL)))
	{
		return was_io_pending();
	}

	return true;
}

bool NetUtils::ReadAsync(SOCKET socket, LPWSAOVERLAPPED overlapped, CHAR* buf, ULONG len)
{
	DWORD dwFlags = 0;
	DWORD dwRecvBytes = 0;
	WSABUF wsaBuf;
	wsaBuf.buf = buf;
	wsaBuf.len = len;
	if (check_sock_error(::WSARecv(socket, &wsaBuf, 1, &dwRecvBytes, &dwFlags, overlapped, NULL)))
	{
		return was_io_pending();
	}

	return true;
}

std::string WSAGetLastErrorMsg()
{
	auto errorMessageID = ::WSAGetLastError();

	char* szMessage = NULL;
	LPSTR messageBuffer = nullptr;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	LocalFree(messageBuffer);

	return message;
}


std::string WSAGetLastErrorMsg(int errCode)
{
	char* szMessage = NULL;
	LPSTR messageBuffer = nullptr;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	LocalFree(messageBuffer);

	return message;
}