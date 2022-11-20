#pragma once

#include "TcpSocket.h"

struct ListenerConfig
{
	uint16 bindPort;
	int32  backLog;
	int32  acceptCount;

	NetworkFactory networkFactory;
	OnAcceptFunc   onAccept;
};

class Listener : public std::enable_shared_from_this<Listener>
{
	friend struct on_accept_t;
private:
	atomic<bool>   _finished;
	ListenerConfig _config;

public:
	Listener(IoService& ioService, ListenerConfig config);
	
	~Listener();

	bool start();

	void stop();

private:
	bool processAccept(const NetworkPtr& session);
	void registerAccpet();

	vector<NetworkPtr> _acceptNetworks;;
	TcpListenerSocket  _listenerSocket;
};