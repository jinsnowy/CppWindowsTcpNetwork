#pragma once

class IoService;
class TcpSocket
{
	friend class IoService;
protected:
	SOCKET	   _socket;
	EndPoint   _endPoint;

private:
	IoService& _ioService;
	atomic<bool> _disposed;

public:
	TcpSocket(IoService& ioService);

	EndPoint getEndPoint() { return _endPoint; }

	virtual ~TcpSocket();

	bool setLinger(SOCKET socket, uint16 onoff, uint16 linger);
	bool setReuseAddress(SOCKET socket, bool flag);
	bool setRecvBufferSize(SOCKET sokcet, int32 size);
	bool setSendBufferSize(SOCKET socket, int32 size);
	bool setTcpNoDelay(SOCKET socket, bool flag);
	bool setUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	void dispose(const char* reason);

protected:
	void close(const char* reason);

	void setEndPoint(const EndPoint& endPoint);
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
	bool read_async(char* buffer, size_t len, AsyncIoCompleteRoutine&& callback)
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

class TcpConnetorSocket : public TcpAsyncSocket
{
public:
	TcpConnetorSocket(IoService& ioService);

	bool connect(const EndPoint& endPoint);

	void disconnect();

	bool reconnect(const EndPoint& endPoint);
};

class TcpListenerSocket : public TcpSocket
{
public:
	TcpListenerSocket(IoService& ioService);

	bool bind(uint16 port);

	bool listen(int32 backLog = SOMAXCONN);

	template<typename OnAcceptCallback>
	bool accept_async(SOCKET acceptSocket, LPVOID* recvBuf, OnAcceptCallback&& onAccept)
	{
		IoEvent* ioEvent = makeAcceptTask(std::forward<OnAcceptCallback>(onAccept));

		if (!NetUtils::AcceptAsync(_socket, acceptSocket, recvBuf, ioEvent))
		{
			ioEvent->release();

			return false;
		}

		return true;
	}
};