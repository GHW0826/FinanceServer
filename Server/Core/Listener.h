#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;
class ServerService;

class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();
public:
	bool StartAccept(ServerServiceRef service);
	void CloseSocket();
public:
	virtual HANDLE GetHandle() override { return reinterpret_cast<HANDLE>(_socket); }
	virtual void Dispatch(class IOCPEvent* iocpEvent, int32 numOfBytes = 0) override;
private:
	// 수신 관련
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
protected:
	SOCKET _socket = INVALID_SOCKET;
	Vector<AcceptEvent*> _acceptEvents;
	ServerServiceRef _service;
};
