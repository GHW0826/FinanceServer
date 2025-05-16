#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;
class ServerService;

class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener()
	{
		SocketUtils::Close(_socket);

		for (AcceptEvent* acceptEvent : _acceptEvents) {
			sdelete<AcceptEvent>(acceptEvent);
		}
	}
public:
	bool StartAccept(ServerServiceRef service)
	{
		_service = service;
		if (_service == nullptr)
			return false;

		_socket = SocketUtils::CreateSocket();
		if (_socket == INVALID_SOCKET)
			return false;

		if (_service->GetIocpCore()->Register(shared_from_this()) == false)
			return false;

		if (SocketUtils::SetReuseAddress(_socket, true) == false)
			return false;

		if (SocketUtils::SetLinger(_socket, 0, 0) == false)
			return false;

		if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
			return false;

		if (SocketUtils::Listen(_socket) == false)
			return false;

		const int32 acceptCount = _service->GetMaxSessionCount();
		for (int32 i = 0; i < acceptCount; ++i) {
			AcceptEvent* acceptEvent = snew<AcceptEvent>();
			acceptEvent->_owner = shared_from_this();
			_acceptEvents.push_back(acceptEvent);
			RegisterAccept(acceptEvent);
		}

		return true;
	}
	void CloseSocket()
	{
		SocketUtils::Close(_socket);
	}
public:
	virtual HANDLE GetHandle() override
	{
		return reinterpret_cast<HANDLE>(_socket);
	}

	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override
	{
		ASSERT_CRASH(iocpEvent->_eventType == EventType::Accept);
		AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
		ProcessAccept(acceptEvent);
	}


private:
	// 수신 관련
	void RegisterAccept(AcceptEvent* acceptEvent)
	{
		SessionRef session = _service->CreateSession(); // Register IOCp;

		acceptEvent->Init();
		acceptEvent->_session = session;

		DWORD bytesReceived = 0;
		if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(),
			session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
			OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent))) {

			const int32 errCode = WSAGetLastError();
			if (errCode != WSA_IO_PENDING) {
				// LOG
				cout << errCode << endl;
				// 일단 다시Accept 걸어줌
				RegisterAccept(acceptEvent);
			}
		}
	}
	void ProcessAccept(AcceptEvent* acceptEvent)
	{
		SessionRef session = acceptEvent->_session;
		if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket)) {
			RegisterAccept(acceptEvent);
			return;
		}

		SOCKADDR_IN sockAddress;
		int32 sizeOfSockAddr = sizeof(sockAddress);
		if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr)) {
			RegisterAccept(acceptEvent);
			return;
		}

		// cout << "Client Connected!" << endl;

		session->SetNetAddress(NetAddress(sockAddress));
		session->ProcessConnect();
		RegisterAccept(acceptEvent);
	}

protected:
	SOCKET _socket = INVALID_SOCKET;
	Vector<AcceptEvent*> _acceptEvents;
	ServerServiceRef _service;
};
