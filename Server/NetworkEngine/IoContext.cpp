#include "pch.h"
#include "IoContext.h"
#include "IoEvent.h"

#define GQCS ::GetQueuedCompletionStatus

IoContext::IoContext()
	:
	_disposed(false),
	_iocpHandle(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL))
{
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE)
}

IoContext::~IoContext()
{
	dispose();
}

void IoContext::registerHandle(HANDLE handle)
{
	_iocpHandle = ::CreateIoCompletionPort(handle, _iocpHandle, NULL, NULL);
	ASSERT_CRASH(_iocpHandle != NULL)
}

void IoContext::dispatch()
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

void IoContext::dispose()
{
	_disposed.store(true);
	::CloseHandle(_iocpHandle);
}
