#pragma once
#include "Session.h"

class ServerSession : public Session
{
public:
	ServerSession(IoService& ioService);
};
