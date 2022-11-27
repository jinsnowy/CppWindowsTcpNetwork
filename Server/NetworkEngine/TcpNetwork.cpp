#include "pch.h"
#include "TcpNetwork.h"
#include "Session.h"
#include "Handshake.h"

struct on_recv_t
{
	shared_ptr<TcpNetwork> network;

	on_recv_t(shared_ptr<TcpNetwork> networkIn)
		: 
		network(networkIn)
	{}

	void operator()(int32 errorCode, DWORD recvBytes)
	{
		if (recvBytes == 0)
		{
			network->disconnectOnError("recv 0");
			return;
		}

		if (errorCode != 0)
		{
			network->handleError(errorCode);
			return;
		}

		network->recv(recvBytes);
		network->registerRecv();
	}
};

struct on_send_t
{
	shared_ptr<TcpNetwork> network;
	std::vector<BufferSegment> segmentHolder;

	on_send_t(shared_ptr<TcpNetwork>  networkIn, std::vector<BufferSegment>&& segments)
		:
		network(networkIn),
		segmentHolder(segments)
	{}

	void operator()(int32 errorCode, DWORD writeBytes)
	{
		if (writeBytes == 0)
		{
			network->disconnectOnError("write 0");
			return;
		}

		if (errorCode != 0)
		{
			network->handleError(errorCode);
			return;
		}

		network->flush();
	}
};

struct on_connect_t
{
	shared_ptr<TcpNetwork> network;
	EndPoint endPoint;

	on_connect_t(shared_ptr<TcpNetwork>  networkIn, EndPoint _endPoint)
		: network(networkIn), endPoint(_endPoint) {}

	void operator()(int32 errorCode, DWORD)
	{
		if (errorCode != 0)
		{
			LOG_ERROR("connect error : %s", get_last_err_msg());
			return;
		}

		network->setConnected(endPoint);
		network->ProcessHandshake();
	}
};

struct on_disconnect_t
{
	shared_ptr<TcpNetwork> network;

	on_disconnect_t(shared_ptr<TcpNetwork> networkIn)
		: 
		network(networkIn) {}

	void operator()(int32 errorCode, DWORD)
	{
		if (errorCode != 0)
		{
			LOG_ERROR("disconnect error : %s", get_last_err_msg());
		}

		network->setDisconnected();
	}
};

TcpNetwork::TcpNetwork(ServiceBase& ServiceBase)
	:
	_socket(ServiceBase),
	_connected(false),
	_pending(false),
	_session()
{
}

TcpNetwork::~TcpNetwork()
{
	_socket.Dispose("network destructor");
}

void TcpNetwork::AttachSession(SessionPtr session)
{
	auto sessionPrev = _session.lock();
	if (sessionPrev)
	{
		sessionPrev->detachNetwork();
	}

	_session = session;

	if (IsConnected())
	{
		session->attachNetwork(shared_from_this());
	}
}

void TcpNetwork::ProcessHandshake()
{
	if (_handshake)
	{
		_handshake->Process();
	}
}

void TcpNetwork::RequireHandshake(Handshake* handshake)
{
	_handshake.reset(handshake);
}

void TcpNetwork::recv(DWORD recvBytes)
{
	if (!_recvBuffer.OnDataRecv(recvBytes))
	{
		disconnectOnError("recv buffer overflows");
		return;
	}
	
	auto session = _session.lock();
	if (session == nullptr)
	{
		disconnectOnError("session disposed");
		return;
	}

	PacketHandler* handler = nullptr;

	while (_recvBuffer.IsHeaderReadable())
	{
		CHAR* bufferToRead = _recvBuffer.GetBufferPtrRead();

		PacketHeader header = PacketHeader::Peek(bufferToRead);

		if (!_recvBuffer.IsReadable(header.size))
		{
			break;
		}

		if (resolvePacketHandler(header.protocol, &handler))
		{
			handler->HandleRecv(session, header, bufferToRead);

			_recvBuffer.Read(header.size);
		}
		else
		{
			disconnectOnError("unknown protocol");
			break;
		}
	}

	_recvBuffer.Rotate();
}

void TcpNetwork::SendAsync(const BufferSegment& segment)
{
	{
		StdWriteLock lock(_sync);

		_pendingSegment.push_back(segment);
	}

	if (_pending.exchange(true) == false)
	{
		flush();
	}
}

void TcpNetwork::DisconnectAsync()
{
	if (false == _socket.DisconnectAsync(on_disconnect_t(shared_from_this())))
	{
		LOG_ERROR("disconnect failed %s", get_last_err_msg());
	}
}

void TcpNetwork::ConnectAsync(const EndPoint& endPoint)
{
	if (NetUtils::SetReuseAddress(_socket.GetSocket(), true) == false)
	{
		LOG_ERROR("connect failed to %s, %s", endPoint.ToString().c_str(), get_last_err_msg());
		return;
	}

	if (NetUtils::BindAnyAddress(_socket.GetSocket(), 0) == false)
	{
		LOG_ERROR("connect failed to %s, %s", endPoint.ToString().c_str(), get_last_err_msg());
		return;
	}

	if (!_socket.ConnectAsync(endPoint, on_connect_t(shared_from_this(), endPoint)))
	{
		LOG_ERROR("connect failed to %s, %s", endPoint.ToString().c_str(), get_last_err_msg());
	}
}

void TcpNetwork::setDisconnected()
{
	if (_connected.exchange(false) == true)
	{
		auto session = _session.lock();
		if (session)
		{
			session->detachNetwork();
		}
	}
}

void TcpNetwork::setConnected(EndPoint endPoint)
{
	if (_connected.exchange(true) == false)
	{
		_endPoint = endPoint;

		auto session = _session.lock();
		if (session)
		{
			session->attachNetwork(shared_from_this());
		}

		LOG_INFO("start recv");

		registerRecv(true);
	}
}

void TcpNetwork::flush()
{
	if (flushInternal() == false)
	{
		int32 errCode = ::WSAGetLastError();

		handleError(errCode);
	}
}

bool TcpNetwork::flushInternal()
{
	if (IsConnected() == false)
		return true;

	std::vector<WSABUF> buffers;
	std::vector<BufferSegment> segments;

	{
		StdWriteLock lock(_sync);

		int32 bufferNum = (int32)_pendingSegment.size();
		if (bufferNum == 0)
		{
			_pending.store(false);
			return true;
		}

		buffers.resize(bufferNum);
		for (int32 i = 0; i < bufferNum; ++i)
		{
			buffers[i] = _pendingSegment[i].wsaBuf();
		}

		segments = std::move(_pendingSegment);
	}
	
	return _socket.WriteAsync(buffers, on_send_t(shared_from_this(), std::move(segments)));
}

void TcpNetwork::registerRecv(bool init)
{
	if (!IsConnected())
		return;

	if (init)
	{
		_recvBuffer.Clear();
	}

	if (!_socket.ReadAsync(_recvBuffer.GetBufferPtr(), _recvBuffer.GetLen(), on_recv_t(shared_from_this())))
	{
		int32 errCode = ::WSAGetLastError();

		handleError(errCode);
	}
}

void TcpNetwork::disconnectOnError(const char* reason)
{
	if (!IsConnected())
		return;

	if (!_socket.DisconnectAsync(on_disconnect_t(shared_from_this())))
	{
		LOG_ERROR("disconnect failed on %s : %s", reason, get_last_err_msg());
	}
}

void TcpNetwork::handleError(int32 errorCode)
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

bool TcpNetwork::resolvePacketHandler(int32 protocol, PacketHandler** handler)
{
	StdReadLock lk(_handlerSync);

	for (auto s_handler : _packetHandlers)
	{
		if (s_handler->IsValidProtocol(protocol))
		{
			*handler = s_handler;
			return true;
		}
	}

	return false;
}
