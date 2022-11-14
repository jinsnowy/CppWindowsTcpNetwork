#pragma once
#include "IoService.h"

struct ServerServiceParam
{
	SessionFactory sessionFactory;
	OnAcceptFunc onAccept;
	uint16 port;
	int backLog;

	ServerServiceParam(SessionFactory _sessionFactory, OnAcceptFunc _onAccept, uint16 _port, int _backLog)
		:
		sessionFactory(_sessionFactory),
		onAccept(_onAccept),
		port(_port),
		backLog(_backLog)
	{}
};

class ServerService : public IoService
{
private:
	SessionFactory _sessionFactory;
	OnAcceptFunc _onAccept;
	uint16 _port;
	int _backLog;

public:
	ServerService(const ServerServiceParam& param);

	virtual void start() override;
	virtual void run() override;
};