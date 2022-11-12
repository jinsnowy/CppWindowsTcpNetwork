#pragma once
#include "IocpCore.h"

class IoService : protected IocpCore
{
	friend class TcpSocket;
public:
	IoService();

	~IoService();

	void start();

	void stop();
};

