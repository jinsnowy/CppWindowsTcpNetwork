#pragma once
#include "Session.h"

class ServerSession : public Session
{
public:
	ServerSession();

protected:
	virtual void onConnected() override;
	virtual void onDisconnected() override;
};
