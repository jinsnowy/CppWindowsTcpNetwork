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
	Dispose();
}

void IoContext::registerHandle(HANDLE handle)
{
	_iocpHandle = ::CreateIoCompletionPort(handle, _iocpHandle, NULL, NULL);
	ASSERT_CRASH(_iocpHandle != NULL)
}

void IoContext::Dispatch(DWORD timeOutMs)
{
	ULONG_PTR ignore = 0;
	IoEvent* ioEvent = 0;
	DWORD numberOfBytesTransferred = 0;

	if (GQCS(_iocpHandle, &numberOfBytesTransferred, &ignore, reinterpret_cast<LPOVERLAPPED*>(&ioEvent), timeOutMs))
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
		if (errCode == WSA_WAIT_TIMEOUT)
		{
			ASSERT_CRASH(ioEvent == nullptr);
			return;
		}

		(*ioEvent)(errCode, numberOfBytesTransferred);
	}

	ioEvent->release();
}

void IoContext::Dispose()
{
	_disposed.store(true);
	::CloseHandle(_iocpHandle);
}
