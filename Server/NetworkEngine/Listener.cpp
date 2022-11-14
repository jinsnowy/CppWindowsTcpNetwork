#include "pch.h"
#include "Listener.h"
#include "Session.h"

struct on_accept_t
{
	shared_ptr<Listener> listenerPtr;
	shared_ptr<Session>  sessionPtr;

	on_accept_t(shared_ptr<Listener> _listenerPtr, shared_ptr<Session> _sessionPtr)
		:
		listenerPtr(_listenerPtr), sessionPtr(_sessionPtr)
	{}

	void operator()(int32 errorCode, DWORD)
	{
		if (listenerPtr->_finished)
		{
			LOG_INFO("listener stopped");
			return;
		}

		if (errorCode != 0)
		{
			LOG_ERROR("OnAccept Error : %s", get_last_err_msg_code(errorCode));
			listenerPtr->registerAccpet();
			return;
		}

		if (!listenerPtr->processAccept(sessionPtr))
		{
			LOG_ERROR("cannot process accept");
			listenerPtr->registerAccpet();
			return;
		}

		listenerPtr->registerAccpet();
	}
};

Listener::Listener(IoService& ioService, ListenerConfig config)
	:
	_finished(false),
	_config(config),
	_listenerSocket(ioService)
{
}

Listener::~Listener()
{
	stop();
}

bool Listener::start()
{
	if (!_listenerSocket.setReuseAddress(true))
	{
		LOG_ERROR("reuse error : %s", get_last_err_msg());
		return false;
	}

	if (!_listenerSocket.setLinger(0, 0))
	{
		LOG_ERROR("set no linger error : %s", get_last_err_msg());
		return false;
	}

	if (!_listenerSocket.bind(_config.bindPort))
	{
		LOG_ERROR("bind %hd error : %s", _config.bindPort, get_last_err_msg());
		return false;
	}

	if (!_listenerSocket.listen(_config.backLog))
	{
		LOG_ERROR("listen error : %s", get_last_err_msg());
		return false;
	}

	for (int32 i = 0; i < _config.acceptCount; ++i)
	{
		registerAccpet();
	}

	LOG_INFO("listen... %s", _listenerSocket.getAddress().toString().c_str());

	return true;
}

void Listener::stop()
{
	_finished = true;

	_listenerSocket.dispose("listener stop");
}

bool Listener::processAccept(const SessionPtr& session)
{
	if (!_listenerSocket.setUpdateAcceptSocket(session->getSocket()))
	{
		LOG_ERROR("cannot update socket : %s", get_last_err_msg());
		return false;
	}

	EndPoint endPoint;
	if (!NetUtils::GetEndPoint(session->getSocket(), endPoint))
	{
		LOG_ERROR("cannot get address : %s", get_last_err_msg());
		return false;
	}

	if (!_config.onAccept(session))
	{
		LOG_ERROR("session rejected");
		return false;
	}

	session->setEndPoint(endPoint);
	session->setConnected();

	return true;
}

void Listener::registerAccpet()
{
	SessionPtr session = _config.sessionFactory(_listenerSocket.ioService());
	LPVOID bufferPtr = session->getRecvBuffer().getBufferPtr();

	if (!_listenerSocket.accept_async(session->getSocket(), bufferPtr, on_accept_t(shared_from_this(), session)))
	{
		if (_finished)
		{
			LOG_INFO("listener stopped");
			return;
		}

		LOG_ERROR("accept error : %s", get_last_err_msg());
	}
}
