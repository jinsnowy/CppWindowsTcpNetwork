#pragma once
#include "Session.h"

class ClientSession : public Session
{
public:
	ClientSession(IoService& ioService);
};

