#include "pch.h"
#include "Session.h"
#include "SessionManager.h"

struct on_recv_t
{
	std::shared_ptr<Session> sessionPtr;

	on_recv_t(std::shared_ptr<Session> session) : sessionPtr(session) {}

	void operator()(int32 errorCode, DWORD recvBytes)
	{
		if (recvBytes == 0)
		{
			sessionPtr->disconnectOnError("recv 0");
			return;
		}

		if (errorCode != 0)
		{
			sessionPtr->handleError(errorCode);
			return;
		}

		sessionPtr->recv(recvBytes);
		sessionPtr->registerRecv();
	}
};

struct on_send_t
{
	std::shared_ptr<Session> sessionPtr;
	std::vector<BufferSegment> segmentHolder;

	on_send_t(std::shared_ptr<Session> session, std::vector<BufferSegment>&& segments)
		: 
		sessionPtr(session),
		segmentHolder(segments)
	{}

	void operator()(int32 errorCode, DWORD writeBytes)
	{
		sessionPtr->_sendIsPending.store(false);

		if (writeBytes == 0)
		{
			sessionPtr->disconnectOnError("write 0");
			return;
		}

		if (errorCode != 0)
		{
			sessionPtr->handleError(errorCode);
			return;
		}

		sessionPtr->flush();
	}
};

struct on_connect_t
{
	std::shared_ptr<Session> sessionPtr;
	EndPoint endPoint;

	on_connect_t(std::shared_ptr<Session> _session, EndPoint _endPoint) 
		: sessionPtr(_session), endPoint(_endPoint) {}

	void operator()(int32 errorCode, DWORD)
	{
		if (errorCode != 0)
		{
			LOG_ERROR("connect error : %s", get_last_err_msg());
			return;
		}

		sessionPtr->setEndPoint(endPoint);
		sessionPtr->setConnected();
	}
};

struct on_disconnect_t
{
	std::shared_ptr<Session> sessionPtr;

	on_disconnect_t(std::shared_ptr<Session> _session)
		: sessionPtr(_session) {}

	void operator()(int32 errorCode, DWORD)
	{
		if (errorCode != 0)
		{
			LOG_ERROR("disconnect error : %s", get_last_err_msg());
		}

		sessionPtr->setDisconnected();
	}
};

Session::Session(IoService& ioService)
	:
	_socket(ioService),
	_connected(false),
	_recvBuffer{}
{
}

Session::~Session()
{
	_socket.dispose("session destructor");
}

void Session::sendAsync(const BufferSegment& segment)
{
	if (isConnected() == false)
		return;

	{
		StdWriteLock lock(_sync);

		_pendingSegment.push_back(segment);
	}

	if (_sendIsPending.exchange(true) == false)
	{
		flush();
	}
}

void Session::setDisconnected()
{
	static SessionManager* sessionManager = SessionManager::getInstance();

	if (_connected.exchange(false) == true)
	{
		sessionManager->removeSession(shared_from_this());

		onDisconnected();
	}
}

void Session::setConnected()
{
	static SessionManager* sessionManager = SessionManager::getInstance();

	if (_connected.exchange(true) == false)
	{
		LOG_INFO("start recv");
		
		registerRecv();

		sessionManager->addSession(shared_from_this());

		onConnected();
	}
}

void Session::handleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		disconnectOnError("connection reset");
		break;
	default:
		LOG_ERROR("handle error %s", get_last_err_msg_code(errorCode));
		break;
	}
}

void Session::recv(DWORD recvBytes)
{
	if (!_recvBuffer.checkWrite(recvBytes))
	{
		disconnectOnError("recv buffer overflows");
		return;
	}

	if (_recvBuffer.isReadable(recvBytes))
	{
		onRecv(recvBytes);

		_recvBuffer.read(recvBytes);
	}

	_recvBuffer.rotate();
}

void Session::disconnect()
{
	if (!isConnected())
		return;
	
	if (!_socket.disconnect_async(on_disconnect_t(shared_from_this())))
	{
		LOG_ERROR("disconnect failed : %s", get_last_err_msg());
		return;
	}

	int count = 0;
	while (isConnected() && ++count < 10)
	{
		std::this_thread::sleep_for(100ms);
	}
}

bool Session::connect(const char* address, uint16 port)
{
	return connect(EndPoint(address, port));
}

bool Session::connect(const EndPoint& endPoint)
{
	if(isConnected())
	{
		LOG_INFO("already connected");
		return true;
	}

	if (NetUtils::SetReuseAddress(_socket.getSocket(), true) == false)
		return false;

	if (NetUtils::BindAnyAddress(_socket.getSocket(), 0) == false)
		return false;

	if (!_socket.connect_async(endPoint, on_connect_t(shared_from_this(), endPoint)))
	{
		LOG_ERROR("connect failed to %s, %s", endPoint.toString().c_str(), get_last_err_msg());
		return false;
	}

	int count = 0;
	while (!isConnected() && ++count < 10)
	{
		std::this_thread::sleep_for(200ms);
	}

	return isConnected(); false;
}

bool Session::reconnect(const EndPoint& endPoint)
{
	if (isConnected())
	{
		disconnect();
	}

	LOG_INFO("reconnect ...");

	return connect(endPoint);
}

bool Session::connectAsync(const EndPoint& endPoint)
{
	if (isConnected())
	{
		LOG_INFO("already connected");
		return true;
	}

	if (NetUtils::SetReuseAddress(_socket.getSocket(), true) == false)
		return false;

	if (NetUtils::BindAnyAddress(_socket.getSocket(), 0) == false)
		return false;

	if (!_socket.connect_async(endPoint, on_connect_t(shared_from_this(), endPoint)))
	{
		LOG_ERROR("connect failed to %s, %s", endPoint.toString().c_str(), get_last_err_msg());
		return false;
	}

	return true;
}

bool Session::reconnectAsync(const EndPoint& endPoint)
{
	if (isConnected())
	{
		disconnect();
	}

	LOG_INFO("reconnect ...");

	return connectAsync(endPoint);
}

void Session::onConnected()
{
	LOG_INFO("connected to %s", getEndPointDesc().c_str());
}

void Session::onDisconnected()
{
	LOG_INFO("disconnected from %s", getEndPointDesc().c_str());
}

void Session::onRecv(DWORD recvBytes)
{
	LOG_INFO("%s", _recvBuffer.getBufferPtrRead());
}

void Session::flush()
{
	if (flushInternal() == false)
	{
		int32 errCode = ::WSAGetLastError();

		handleError(errCode);
	}
}

bool Session::flushInternal()
{
	std::vector<WSABUF> buffers;
	std::vector<BufferSegment> segments;

	{
		StdWriteLock lock(_sync);

		int32 bufferNum = (int32)_pendingSegment.size();
		if (bufferNum == 0)
			return true;

		buffers.resize(bufferNum);
		for (int32 i = 0; i < bufferNum; ++i)
		{
			buffers[i] = _pendingSegment[i].wsaBuf();
		}

		segments = std::move(_pendingSegment);
	}
	
	LOG_INFO("send %d buffers", (int)buffers.size());

	return _socket.write_async(buffers, on_send_t(shared_from_this(), std::move(segments)));
}

void Session::registerRecv()
{
	if (!isConnected())
		return;

	if (!_socket.read_async(_recvBuffer.getBufferPtr(), _recvBuffer.getLen(), on_recv_t(shared_from_this())))
	{
		int32 errCode = ::WSAGetLastError();
		
		handleError(errCode);
	}
}

void Session::disconnectOnError(const char* reason)
{
	if (!isConnected())
		return;

	if (!_socket.disconnect_async(on_disconnect_t(shared_from_this())))
	{
		LOG_ERROR("disconnect failed on %s : %s", reason, get_last_err_msg());
	}
}
