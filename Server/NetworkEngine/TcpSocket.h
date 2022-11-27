#pragma once

class ServiceBase;
class TcpSocket
{
	friend class ServiceBase;
	friend class TcpListenerSocket;
protected:
	SOCKET	   _socket;

private:
	ServiceBase& _service;
	atomic<bool> _disposed;

public:
	TcpSocket(ServiceBase& service);
	
	SOCKET   GetSocket() { return _socket; }

	virtual ~TcpSocket();

	bool SetLinger(uint16 onoff, uint16 linger);
	bool SetReuseAddress(bool flag);
	bool SetRecvBufferSize(int32 size);
	bool SetSendBufferSize(int32 size);
	bool SetTcpNoDelay(bool flag);

	void Dispose(const char* reason);
	void Close(const char* reason);
};

class TcpAsyncSocket : public TcpSocket
{
public:
	TcpAsyncSocket(ServiceBase& service);

	template<typename AsyncIoCompleteRoutine>
	bool WriteAsync(char* buffer, DWORD len, AsyncIoCompleteRoutine&& callback)
	{
		IoEvent* ioEvent = makeWriteTask(std::forward<AsyncIoCompleteRoutine>(callback));
	
		if (!NetUtils::WriteAsync(_socket, reinterpret_cast<LPWSAOVERLAPPED>(ioEvent), buffer, len))
		{
			ioEvent->release();
			return false;
		}

		return true;
	}

	template<typename AsyncIoCompleteRoutine>
	bool WriteAsync(vector<WSABUF> buffers, AsyncIoCompleteRoutine&& callback)
	{
		IoEvent* ioEvent = makeWriteTask(std::forward<AsyncIoCompleteRoutine>(callback));

		if (!NetUtils::WriteAsync(_socket, reinterpret_cast<LPWSAOVERLAPPED>(ioEvent), buffers.data(), (DWORD)buffers.size()))
		{
			ioEvent->release();

			return false;
		}

		return true;
	}

	template<typename AsyncIoCompleteRoutine>
	bool ReadAsync(char* buffer, ULONG len, AsyncIoCompleteRoutine&& callback)
	{
		IoEvent* ioEvent = makeReadTask(std::forward<AsyncIoCompleteRoutine>(callback));

		if (!NetUtils::ReadAsync(_socket, reinterpret_cast<LPWSAOVERLAPPED>(ioEvent), buffer, len))
		{
			ioEvent->release();

			return false;
		}

		return true;
	}


	template<typename AsyncIoCompleteRoutine>
	bool ConnectAsync(const EndPoint& endPoint, AsyncIoCompleteRoutine&& callback)
	{
		IoEvent* ioEvent = makeConnectTask(std::forward<AsyncIoCompleteRoutine>(callback));

		if (!NetUtils::ConnectAsync(_socket, reinterpret_cast<LPWSAOVERLAPPED>(ioEvent), endPoint))
		{
			ioEvent->release();

			return false;
		}

		return true;
	}

	template<typename AsyncIoCompleteRoutine>
	bool DisconnectAsync(AsyncIoCompleteRoutine&& callback)
	{
		IoEvent* ioEvent = makeDisconnectTask(std::forward<AsyncIoCompleteRoutine>(callback));

		if (!NetUtils::DisconnectAsync(_socket, reinterpret_cast<LPWSAOVERLAPPED>(ioEvent)))
		{
			ioEvent->release();

			return false;
		}

		return true;
	}
};

class TcpActiveSocket : public TcpAsyncSocket
{
public:
	TcpActiveSocket(ServiceBase& service);
};

class TcpListenerSocket : public TcpSocket
{
private:
	EndPoint _bindEndPoint;

public:
	TcpListenerSocket(ServiceBase& service);

	EndPoint GetAddress() { return _bindEndPoint; }

	bool Bind(uint16 port);

	bool Listen(int32 backLog = SOMAXCONN);

	bool SetUpdateAcceptSocket(SOCKET acceptSocket);

	template<typename OnAcceptCallback>
	bool AcceptAsync(SOCKET acceptSocket, LPVOID recvBuf, OnAcceptCallback&& onAccept)
	{
		IoEvent* ioEvent = makeAcceptTask(std::forward<OnAcceptCallback>(onAccept));

		if (!NetUtils::AcceptAsync(_socket, acceptSocket, recvBuf, ioEvent))
		{
			ioEvent->release();

			return false;
		}

		return true;
	}

	ServiceBase& GetServiceBase() { return TcpSocket::_service; }
};