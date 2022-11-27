#include "pch.h"
#include "TcpSocket.h"
#include "ServiceBase.h"

TcpSocket::TcpSocket(ServiceBase& service)
	:
	_service(service),
	_socket(NetUtils::CreateSocket())
{
	_service.GetContext().registerHandle((HANDLE)_socket);
}

TcpSocket::~TcpSocket()
{
	Dispose("destructor");
}

bool TcpSocket::SetLinger(uint16 onoff, uint16 linger)
{
	return NetUtils::SetLinger(_socket, onoff, linger);
}

bool TcpSocket::SetReuseAddress(bool flag)
{
	return NetUtils::SetReuseAddress(_socket, flag);
}

bool TcpSocket::SetRecvBufferSize(int32 size)
{
	return NetUtils::SetRecvBufferSize(_socket, size);
}

bool TcpSocket::SetSendBufferSize(int32 size)
{
	return NetUtils::SetSendBufferSize(_socket, size);
}

bool TcpSocket::SetTcpNoDelay(bool flag)
{
	return NetUtils::SetTcpNoDelay(_socket, flag);
}

void TcpSocket::Dispose(const char* reason)
{
	if (_disposed.exchange(true) == false)
	{
		LOG_INFO("disposed : %s", reason);

		NetUtils::Close(_socket);
	}
}

void TcpSocket::Close(const char* reason)
{
	LOG_INFO("close : %s", reason);

	NetUtils::Shutdown(_socket, SD_BOTH);
}

TcpAsyncSocket::TcpAsyncSocket(ServiceBase& service)
	:
	TcpSocket(service)
{
}

TcpActiveSocket::TcpActiveSocket(ServiceBase& service)
	:
	TcpAsyncSocket(service)
{
}

TcpListenerSocket::TcpListenerSocket(ServiceBase& service)
	:
	TcpSocket(service)
{
}

bool TcpListenerSocket::Bind(uint16 port)
{
	_bindEndPoint = EndPoint("127.0.0.1", port);
	return NetUtils::Bind(_socket, _bindEndPoint);
}

bool TcpListenerSocket::Listen(int32 backLog)
{
	return NetUtils::Listen(_socket, backLog);
}

bool TcpListenerSocket::SetUpdateAcceptSocket(SOCKET acceptSocket)
{
	return NetUtils::SetUpdateAcceptSocket(acceptSocket, _socket);
}