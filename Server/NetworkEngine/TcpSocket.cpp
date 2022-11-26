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
	dispose("destructor");
}

bool TcpSocket::setLinger(uint16 onoff, uint16 linger)
{
	return NetUtils::SetLinger(_socket, onoff, linger);
}

bool TcpSocket::setReuseAddress(bool flag)
{
	return NetUtils::SetReuseAddress(_socket, flag);
}

bool TcpSocket::setRecvBufferSize(int32 size)
{
	return NetUtils::SetRecvBufferSize(_socket, size);
}

bool TcpSocket::setSendBufferSize(int32 size)
{
	return NetUtils::SetSendBufferSize(_socket, size);
}

bool TcpSocket::setTcpNoDelay(bool flag)
{
	return NetUtils::SetTcpNoDelay(_socket, flag);
}

void TcpSocket::dispose(const char* reason)
{
	if (_disposed.exchange(true) == false)
	{
		LOG_INFO("disposed : %s", reason);

		NetUtils::Close(_socket);
	}
}

void TcpSocket::close(const char* reason)
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

bool TcpListenerSocket::bind(uint16 port)
{
	_bindEndPoint = EndPoint("127.0.0.1", port);
	return NetUtils::Bind(_socket, _bindEndPoint);
}

bool TcpListenerSocket::listen(int32 backLog)
{
	return NetUtils::Listen(_socket, backLog);
}

bool TcpListenerSocket::setUpdateAcceptSocket(SOCKET acceptSocket)
{
	return NetUtils::SetUpdateAcceptSocket(acceptSocket, _socket);
}