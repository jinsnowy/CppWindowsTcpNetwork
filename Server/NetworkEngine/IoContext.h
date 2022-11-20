#pragma once

class IoObject;
class IoContext
{
private:
	atomic<bool> _disposed;
	HANDLE _iocpHandle;

public:
	IoContext();
	~IoContext();

	void registerHandle(HANDLE handle);

	void dispatch();

	void dispose();

	bool isFinished() { return _disposed; }
};

