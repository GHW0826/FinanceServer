#pragma once

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send
};

class IOCPEvent : public OVERLAPPED
{
public:
	IOCPEvent(EventType type)
	  : _eventType(type)
  {
  	Init();
  }

	void Init()
  {
  	OVERLAPPED::hEvent = 0;
  	OVERLAPPED::Internal = 0;
  	OVERLAPPED::InternalHigh = 0;
  	OVERLAPPED::Offset = 0;
  	OVERLAPPED::OffsetHigh = 0;
  }
public:
	EventType _eventType;
	IocpObjectRef _owner;
};

// ConenctEvent
class ConnectEvent : public IOCPEvent
{
public:
	ConnectEvent() 
		: IOCPEvent(EventType::Connect)
	{
	}
};

// DisconenctEvent
class DisconnectEvent : public IOCPEvent
{
public:
	DisconnectEvent() 
		: IOCPEvent(EventType::Disconnect)
	{
	}
};


// AcceptEvent
class AcceptEvent : public IOCPEvent
{
public:
	AcceptEvent() 
		: IOCPEvent(EventType::Accept)
	{
	}
public:
	SessionRef _session = nullptr;
};


// RecvEvent
class RecvEvent : public IOCPEvent
{
public:
	RecvEvent() 
		: IOCPEvent(EventType::Recv)
	{
	}
};

// SendEvent
class SendEvent : public IOCPEvent
{
public:
	SendEvent() 
		: IOCPEvent(EventType::Send)
	{
	}

	Vector<SendBufferRef> _sendBuffers;
};
