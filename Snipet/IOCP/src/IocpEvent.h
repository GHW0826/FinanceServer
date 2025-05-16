#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	// PreRecv, 0byte recv
	Recv,
	Send
};

// 가상함수 사용 x
// 가상함수 테이블이 맨앞에 위치해,
// OVERLAPPED 포인터 원래 위치의 데이터가 오염될 수 있음.
// OVERLAPPED EX 역할
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
public:
	EventType _eventType;
	IocpObjectRef _owner;
};

// ConenctEvent
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() 
		: IocpEvent(EventType::Connect)
	{
	}
};

// DisconenctEvent
class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() 
		: IocpEvent(EventType::Disconnect)
	{
	}
};


// AcceptEvent
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() 
		: IocpEvent(EventType::Accept)
	{
	}
public:
	SessionRef _session = nullptr;
};


// RecvEvent
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() 
		: IocpEvent(EventType::Recv)
	{
	}
};

// SendEvent
class SendEvent : public IocpEvent
{
public:
	SendEvent() 
		: IocpEvent(EventType::Send)
	{
	}

	Vector<SendBufferRef> _sendBuffers;
};
