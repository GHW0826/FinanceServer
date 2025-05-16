#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IOCPCore::IOCPCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IOCPCore::~IOCPCore()
{
	::CloseHandle(_iocpHandle);
}

bool IOCPCore::Register(IocpObjectRef iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /* key */ 0, 0);
}

// 워커 스레드들이 아래 Dispatch함수를 계속 실행해서 일감이 있는지 확인.
bool IOCPCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IOCPEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs)) {
		// cout << "IocpCore::Dispatch" << endl;
		IocpObjectRef iocpObject = iocpEvent->_owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	} else {
		int32 errCode = ::WSAGetLastError();
		switch (errCode) {
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO 로그 찍기
			cout << errCode << endl;
			IocpObjectRef iocpObject = iocpEvent->_owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}
	return true;
}


void IocpObject::Dispatch(IOCPEvent* iocpEvent, int32 numOfBytes)
{
}
