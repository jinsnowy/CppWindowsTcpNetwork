#pragma once

class IoService;
class TcpSocket
{
	friend class IoService;
	friend class TcpListenerSocket;
protected:
	SOCKET	   _socket;

private:
	IoService& _ioService;
	atomic<bool> _disposed;

public:
	TcpSocket(IoService& ioService);
	
	SOCKET   getSocket() { return _socket; }

	virtual ~TcpSocket();

	bool setLinger(uint16 onoff, uint16 linger);
	bool setReuseAddress(bool flag);
	bool setRecvBufferSize(int32 size);
	bool setSendBufferSize(int32 size);
	bool setTcpNoDelay(bool flag);

	void dispose(const char* reason);
	void close(const char* reason);
};

class TcpAsyncSocket : public TcpSocket
{
public:
	TcpAsyncSocket(IoService& ioService);

	template<typename AsyncIoCompleteRoutine>
	bool write_async(char* buffer, DWORD len, AsyncIoCompleteRoutine&& callback)
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
	bool write_async(vector<WSABUF> buffers, AsyncIoCompleteRoutine&& callback)
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
	bool read_async(char* buffer, ULONG len, AsyncIoCompleteRoutine&& callback)
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
	bool connect_async(const EndPoint& endPoint, AsyncIoCompleteRoutine&& callback)
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
	bool disconnect_async(AsyncIoCompleteRoutine&& callback)
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
	TcpActiveSocket(IoService& ioService);
};

class TcpListenerSocket : public TcpSocket
{
private:
	EndPoint _bindEndPoint;

public:
	TcpListenerSocket(IoService& ioService);

	EndPoint getAddress() { return _bindEndPoint; }

	bool bind(uint16 port);

	bool listen(int32 backLog = SOMAXCONN);

	bool setUpdateAcceptSocket(SOCKET acceptSocket);

	template<typename OnAcceptCallback>
	bool accept_async(SOCKET acceptSocket, LPVOID recvBuf, OnAcceptCallback&& onAccept)
	{
		IoEvent* ioEvent = makeAcceptTask(std::forward<OnAcceptCallback>(onAccept));

		if (!NetUtils::AcceptAsync(_socket, acceptSocket, recvBuf, ioEvent))
		{
			ioEvent->release();

			return false;
		}

		return true;
	}

	IoService& ioService() { return TcpSocket::_ioService; }
};