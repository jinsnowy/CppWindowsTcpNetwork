#include "pch.h"
#include "TcpSocket.h"
#include "IoService.h"

TcpSocket::TcpSocket(IoService& ioService)
	:
	_ioService(ioService),
	_socket(NetUtils::CreateSocket())
{
	_ioService.registerHandle((HANDLE)_socket);
}

TcpSocket::~TcpSocket()
{
	dispose("destructor");
}

bool TcpSocket::setLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	return NetUtils::SetLinger(socket, onoff, linger);
}

bool TcpSocket::setReuseAddress(SOCKET socket, bool flag)
{
	return NetUtils::SetReuseAddress(socket, flag);
}

bool TcpSocket::setRecvBufferSize(SOCKET sokcet, int32 size)
{
	return NetUtils::SetRecvBufferSize(sokcet, size);
}

bool TcpSocket::setSendBufferSize(SOCKET socket, int32 size)
{
	return NetUtils::SetSendBufferSize(socket, size);
}

bool TcpSocket::setTcpNoDelay(SOCKET socket, bool flag)
{
	return NetUtils::SetTcpNoDelay(socket, flag);
}

bool TcpSocket::setUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return NetUtils::SetUpdateAcceptSocket(socket, listenSocket);
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
}

void TcpSocket::setEndPoint(const EndPoint& endPoint)
{
	_endPoint = endPoint;
}

TcpAsyncSocket::TcpAsyncSocket(IoService& ioService)
	:
	TcpSocket(ioService)
{
}

TcpConnetorSocket::TcpConnetorSocket(IoService& ioService)
	:
	TcpAsyncSocket(ioService)
{
}

bool TcpConnetorSocket::connect(const EndPoint& endPoint)
{
	return false;
}

void TcpConnetorSocket::disconnect()
{
}

bool TcpConnetorSocket::reconnect(const EndPoint& endPoint)
{
	return false;
}

TcpListenerSocket::TcpListenerSocket(IoService& ioService)
	:
	TcpSocket(ioService)
{
}

bool TcpListenerSocket::bind(uint16 port)
{
	return NetUtils::Bind(_socket, EndPoint("127.0.0.1", port));
}

bool TcpListenerSocket::listen(int32 backLog)
{
	return NetUtils::Listen(_socket, backLog);
}