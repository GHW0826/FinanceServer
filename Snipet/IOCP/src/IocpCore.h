#pragma once

class IocpCore
{
public:
	IocpCore()
  {
  	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
  	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
  }
	~IocpCore()
  {
  	::CloseHandle(_iocpHandle);
  }
public:
	inline HANDLE GetHandle() { return _iocpHandle; }

  bool Register(IocpObjectRef iocpObject)
  {
  	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /* key  */ 0, 0);
  }

	bool Dispatch(uint32 timeoutMs = INFINITE)
  {  
  	DWORD numOfBytes = 0;
  	ULONG_PTR key = 0;
  	IocpEvent* iocpEvent = nullptr;
  	if (::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs)) {
  		IocpObjectRef iocpObject = iocpEvent->_owner;
  		iocpObject->Dispatch(iocpEvent, numOfBytes);
  	}
    else {
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


private:
	HANDLE _iocpHandle;
};
