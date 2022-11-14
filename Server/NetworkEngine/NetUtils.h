#pragma once

#include "EndPoint.h"

class NetUtils
{
private:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;

public:
	static void initialize();

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static SOCKET CreateSocket();

	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET sokcet, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	static bool Bind(SOCKET socket, EndPoint netAddr);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool GetEndPoint(SOCKET socket, EndPoint& endPoint);
	static bool Listen(SOCKET socket, int32 backLog = SOMAXCONN);
	static void Shutdown(SOCKET socket, int32 how);
	static void Close(SOCKET socket);

	static bool ConnectAsync(SOCKET socket, LPWSAOVERLAPPED overlapped, const EndPoint& endPoint);
	static bool DisconnectAsync(SOCKET socket, LPWSAOVERLAPPED overlapped);
	static bool AcceptAsync(SOCKET listenSocket, SOCKET acceptSocket, LPVOID recvBuf, LPWSAOVERLAPPED overlapped);

	static bool WriteAsync(SOCKET socket, LPWSAOVERLAPPED overlapped, CHAR* buf, ULONG len);
	static bool WriteAsync(SOCKET socket, LPWSAOVERLAPPED overlapped, WSABUF* buffers, DWORD bufferLen);
	static bool ReadAsync(SOCKET socket, LPWSAOVERLAPPED overlapped, CHAR* buf, ULONG len);
};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}

std::string WSAGetLastErrorMsg();
std::string WSAGetLastErrorMsg(int errCode);