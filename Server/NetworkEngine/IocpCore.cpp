#include "pch.h"
#include "IocpCore.h"
#include "IoEvent.h"

#define GQCS ::GetQueuedCompletionStatus

IocpCore::IocpCore()
	:
	_disposed(false),
	_iocpHandle(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL))
{
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE)
}

IocpCore::~IocpCore()
{
	dispose();
}

void IocpCore::registerHandle(HANDLE handle)
{
	_iocpHandle = ::CreateIoCompletionPort(handle, _iocpHandle, NULL, NULL);
	ASSERT_CRASH(_iocpHandle != NULL)
}

void IocpCore::dispatch()
{
	ULONG_PTR ignore = 0;
	IoEvent* ioEvent = 0;
	DWORD numberOfBytesTransferred = 0;

	while (!_disposed)
	{
		if (GQCS(_iocpHandle, &numberOfBytesTransferred, &ignore, reinterpret_cast<LPOVERLAPPED*>(&ioEvent), INFINITE))
		{
			(*ioEvent)(0, numberOfBytesTransferred);
		}
		else
		{
			if (_disposed)
			{
				LOG_INFO("iocp ends");
				return;
			}

			int32 errCode = ::WSAGetLastError();
			(*ioEvent)(errCode, numberOfBytesTransferred);
		}

		ioEvent->release();
	}
}

void IocpCore::dispose()
{
	_disposed.store(true);
	::CloseHandle(_iocpHandle);
}
