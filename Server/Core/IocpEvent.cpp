#include "pch.h"
#include "IocpEvent.h"

IOCPEvent::IOCPEvent(EventType type)
	: _eventType(type)
{
	Init();
}

void IOCPEvent::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
