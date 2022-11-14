#pragma once

class IoObject;
class IocpCore
{
private:
	atomic<bool> _disposed;
	HANDLE _iocpHandle;

public:
	IocpCore();
	~IocpCore();

	void registerHandle(HANDLE handle);

	void dispatch();

	void dispose();

	bool isFinished() { return _disposed; }
};

