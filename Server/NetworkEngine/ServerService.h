#pragma once
#include "IoService.h"

struct ServerServiceParam
{
	uint16 port;
	int backLog;

	ServerServiceParam(uint16 _port, int _backLog)
		:
		port(_port),
		backLog(_backLog)
	{}
};

class LoginSession;
class ServerService : public IoService
{
private:
	uint16 _port;
	int _backLog;

public:
	ServerService(const ServerServiceParam& param);

	virtual void Start() override;

	virtual void Run() override;
};