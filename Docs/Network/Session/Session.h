#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IOCPCore;
	friend class Service;

	enum {
		BUFFER_SIZE = 0x10000 // 64KB
	};

public:
	Session()
		: _recvBuffer(BUFFER_SIZE)
	{
		_socket = SocketUtils::CreateSocket();
	}

	virtual ~Session()
	{
		SocketUtils::Close(_socket);
	}
public:
	void Send(SendBufferRef sendBuffer)
	{
		if (IsConnected() == false) {
			cout << "is not connected" << endl;
			return;
		}

		bool registerSend = false;
		{
			WRITE_LOCK;

			_sendQueue.push(sendBuffer);
			if (_sendRegistered.exchange(true) == false)
				registerSend = true;
		}

		if (registerSend) {
			RegisterSend();
		}
	}

	bool Connect()
	{
		return RegisterConnect();
	}

	void Disconnect(const WCHAR* cause)
	{
		if (_connected.exchange(false) == false)
			return;

		RegisterDiconnect();
	}

	shared_ptr<Service> GetService() { return _service.lock(); }
	void SetService(shared_ptr<Service> service) { _service = service; }

public:
	// 정보 관련
	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }
	bool IsConnected() { return _connected; }
	SessionRef GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
private:
	// 인터페이스 구현
	virtual HANDLE GetHandle() override { return reinterpret_cast<HANDLE>(_socket); }

	virtual void Dispatch(class IOCPEvent* iocpEvent, int32 numOfBytes = 0) override
	{
		switch (iocpEvent->_eventType) {
		case EventType::Connect:
			ProcessConnect();
			break;
		case EventType::Disconnect:
			ProcessDisconnect();
			break;
		case EventType::Recv:
			ProcessRecv(numOfBytes);
			break;
		case EventType::Send:
			ProcessSend(numOfBytes);
			break;
		default:
			break;
		}
	}

private:
	// 전송 관련
	bool RegisterConnect()
	{
		if (IsConnected())
			return false;

		if (GetService()->GetServiceType() != ServiceType::Client)
			return false;

		if (SocketUtils::SetReuseAddress(_socket, true) == false)
			return false;

		if (SocketUtils::BindAnyAddress(_socket, 0) == false)
			return false;

		_connectEvent.Init();
		_connectEvent._owner = shared_from_this(); // add ref

		DWORD numOfBytes = 0;
		SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
		if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent)) {
			int32 errCode = ::WSAGetLastError();
			if (errCode != WSA_IO_PENDING) {
				cout << errCode << endl;
				_connectEvent._owner = nullptr; // release ref
				return false;
			}
		}
		return true;
	}

	bool RegisterDiconnect()
	{
		_disconnectEvent.Init();
		_disconnectEvent._owner = shared_from_this(); // add ref
		if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0)) {
			int32 errCode = ::WSAGetLastError();
			if (errCode != WSA_IO_PENDING) {
				cout << errCode << endl;
				_disconnectEvent._owner = nullptr; // release ref
				return false;
			}
		}

		return true;
	}

	void RegisterRecv()
	{
		if (IsConnected() == false)
			return;

		_recvEvent.Init();
		_recvEvent._owner = shared_from_this(); // ADD Ref

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
		wsaBuf.len = _recvBuffer.FreeSize();

		DWORD numOfBytes = 0;
		DWORD flags = 0;
		if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr)) {
			int32 errCode = ::WSAGetLastError();
			if (errCode != WSA_IO_PENDING) {
				cout << errCode << endl;
				HandleError(errCode);
				_recvEvent._owner = nullptr; // Release Ref
			}
		}
	}

	void RegisterSend()
	{
		if (IsConnected() == false) {
			return;
		}

		_sendEvent.Init();
		_sendEvent._owner = shared_from_this(); // ADD Ref
		{
			WRITE_LOCK;
			int32 writeSize = 0;
			while (_sendQueue.empty() == false) {
				SendBufferRef sendBuffer = _sendQueue.front();

				writeSize += sendBuffer->WriteSize();

				_sendQueue.pop();
				_sendEvent._sendBuffers.push_back(sendBuffer);
			}
		}

		Vector<WSABUF> wsaBufs;
		wsaBufs.reserve(_sendEvent._sendBuffers.size());
		for (SendBufferRef sendBuffer : _sendEvent._sendBuffers) {
			WSABUF wsaBuf;
			wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
			wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
			wsaBufs.push_back(wsaBuf);
		}

		DWORD numOfBytes = 0;
		if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr)) {
			int32 errCode = ::WSAGetLastError();
			if (errCode != WSA_IO_PENDING) {
				cout << errCode << endl;
				HandleError(errCode);
				_sendEvent._owner = nullptr; // release ref
				_sendEvent._sendBuffers.clear(); // release ref
				_sendRegistered.store(false);
			}
		}
	}
	
	void ProcessConnect()
	{
		_connectEvent._owner = nullptr; // release ref
		_connected.store(true);

		// 세션 등록
		GetService()->AddSession(GetSessionRef());

		// 콘텐츠 코드에서 오버로딩
		OnConnected();

		// 수신 등록
		RegisterRecv();
	}

	void ProcessDisconnect()
	{
		_disconnectEvent._owner = nullptr; // release ref

		OnDisconnected(); // 컨텐츠 코드에서 오버로딩
		GetService()->ReleaseSession(GetSessionRef());
	}

	void ProcessRecv(int32 numOfBytes)
	{
		_recvEvent._owner = nullptr; // Release Ref

		if (numOfBytes == 0) {
			Disconnect(L"Recv 0)");
			return;
		}

		if (_recvBuffer.OnWrite(numOfBytes) == false) {
			Disconnect(L"OnWrite Overflow");
			return;
		}

		// 컨텐츠
		int32 dataSize = _recvBuffer.DataSize();
		int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
		if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false) {
			Disconnect(L"OnRead Overflow");
			return;
		}

		// 커서 정리
		_recvBuffer.Clean();

		// 수신 등록
		RegisterRecv();
	}

	void ProcessSend(int32 numOfBytes)
	{
		_sendEvent._owner = nullptr; // release ref
		_sendEvent._sendBuffers.clear();
		if (numOfBytes == 0) {
			Disconnect(L"Send 0");
			return;
		}

		// 컨텐츠 코드에서 오버로딩
		OnSend(numOfBytes);

		WRITE_LOCK;
		if (_sendQueue.empty())
			_sendRegistered.store(false);
		else
			RegisterSend();
	}

	void HandleError(int32 errCode)
	{
		switch (errCode) {
		case WSAECONNRESET:
		case WSAECONNABORTED:
			Disconnect(L"HandleError");
			break;
		default:
			cout << "Handle Error: " << errCode << endl;
			break;
		}
	}

protected:
	// 컨텐츠 코드에서 오버로딩
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}
	virtual void OnDisconnected() {}
private:
	weak_ptr<Service> _service;
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false;
private:
	USE_LOCK;
	// 수신
	RecvBuffer _recvBuffer;
	// 송신
	Queue<SendBufferRef> _sendQueue;
	Atomic<bool> _sendRegistered = false;
private:
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
};

