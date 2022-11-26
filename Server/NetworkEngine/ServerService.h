#pragma once
#include "ServiceBase.h"
#include "ServerSession.h"

struct ServerServiceParam
{
	uint16 port;
	int backLog;
	ServerSessionFactory sessionFactory;

	ServerServiceParam(uint16 _port, int _backLog, ServerSessionFactory _sessionFactory)
		:
		port(_port),
		backLog(_backLog),
		sessionFactory(_sessionFactory)
	{}
};

class LoginSession;
class ServerService : public ServiceBase
{
private:
	uint16 _port;
	int _backLog;
	ServerSessionFactory _sessionFactory;

public:
	ServerService(const ServerServiceParam& param);

	virtual void Start() override;
};