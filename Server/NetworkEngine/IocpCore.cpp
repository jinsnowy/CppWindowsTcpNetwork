#include "pch.h"
#include "IocpCore.h"
#include "IoObject.h"
#include "IoEvent.h"

#define GQCS ::GetQueuedCompletionStatus

IocpCore::IocpCore()
	:
	_iocpHandle(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL))
{
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE)
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

void IocpCore::registerHandle(HANDLE handle)
{
	_iocpHandle = ::CreateIoCompletionPort(handle, _iocpHandle, NULL, NULL);
}

void IocpCore::dispatch()
{
	DWORD numberOfBytesTransferred = 0;
	ULONG_PTR ignore = 0;
	IoEvent* ioEvent = 0;
	if (GQCS(_iocpHandle, &numberOfBytesTransferred, &ignore, reinterpret_cast<LPOVERLAPPED*>(&ioEvent), INFINITE))
	{
		(*ioEvent)(0, numberOfBytesTransferred);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		(*ioEvent)(errCode, numberOfBytesTransferred);
	}

	ioEvent->release();
}