#pragma once
#include "Session.h"

class ServerSession : public Session
{
	friend class LoginSessionManager;
public:
	ServerSession();

protected:
	virtual void onConnected() override;

	virtual void onDisconnected() override;
};
