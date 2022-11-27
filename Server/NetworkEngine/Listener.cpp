#include "pch.h"
#include "Listener.h"
#include "TcpNetwork.h"

struct on_accept_t
{
	shared_ptr<Listener> listenerPtr;
	shared_ptr<TcpNetwork> networkPtr;

	on_accept_t(shared_ptr<Listener> _listenerPtr, shared_ptr<TcpNetwork> _networkPtr)
		:
		listenerPtr(_listenerPtr), networkPtr(_networkPtr)
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

		if (!listenerPtr->processAccept(networkPtr))
		{
			LOG_ERROR("cannot process accept");
			listenerPtr->registerAccpet();
			return;
		}

		listenerPtr->registerAccpet();
	}
};

Listener::Listener(ServiceBase& ServiceBase, ListenerConfig config)
	:
	_finished(false),
	_config(config),
	_listenerSocket(ServiceBase)
{
}

Listener::~Listener()
{
	stop();
}

bool Listener::start()
{
	if (!_listenerSocket.SetReuseAddress(true))
	{
		LOG_ERROR("reuse error : %s", get_last_err_msg());
		return false;
	}

	if (!_listenerSocket.SetLinger(0, 0))
	{
		LOG_ERROR("set no linger error : %s", get_last_err_msg());
		return false;
	}

	if (!_listenerSocket.Bind(_config.bindPort))
	{
		LOG_ERROR("bind %hd error : %s", _config.bindPort, get_last_err_msg());
		return false;
	}

	if (!_listenerSocket.Listen(_config.backLog))
	{
		LOG_ERROR("listen error : %s", get_last_err_msg());
		return false;
	}

	for (int32 i = 0; i < _config.acceptCount; ++i)
	{
		registerAccpet();
	}

	LOG_INFO("listen... %s", _listenerSocket.GetAddress().ToString().c_str());

	return true;
}

void Listener::stop()
{
	_finished = true;

	_listenerSocket.Dispose("listener stop");
}

bool Listener::processAccept(const NetworkPtr& network)
{
	if (!_listenerSocket.SetUpdateAcceptSocket(network->GetSocket()))
	{
		LOG_ERROR("cannot update socket : %s", get_last_err_msg());
		return false;
	}

	EndPoint endPoint;
	if (!NetUtils::GetEndPoint(network->GetSocket(), endPoint))
	{
		LOG_ERROR("cannot get address : %s", get_last_err_msg());
		return false;
	}

	if (!_config.onAccept(network))
	{
		LOG_ERROR("session rejected");
		return false;
	}

	network->setConnected(endPoint);

	return true;
}

void Listener::registerAccpet()
{
	NetworkPtr network = _config.networkFactory(_listenerSocket.GetServiceBase());
	LPVOID bufferPtr = network->GetRecvBuffer().GetBufferPtr();

	if (!_listenerSocket.AcceptAsync(network->GetSocket(), bufferPtr, on_accept_t(shared_from_this(), network)))
	{
		if (_finished)
		{
			LOG_INFO("listener stopped");
			return;
		}

		LOG_ERROR("accept error : %s", get_last_err_msg());
	}
}
