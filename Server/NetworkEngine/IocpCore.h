#pragma once

class IoObject;
class IocpCore
{
private:
	HANDLE _iocpHandle;

public:
	IocpCore();
	~IocpCore();

	void registerHandle(HANDLE handle);

	void dispatch();
};

