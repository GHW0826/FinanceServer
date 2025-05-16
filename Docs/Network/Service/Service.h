#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client
};

using SessionFactory = function<SessionRef(void)>;

class Service : public enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, IOCPCoreRef core, SessionFactory factory, int32 maxSessionCount = 1)
		: _type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
	{
	}
	virtual ~Service()
	{
	}
public:
	virtual bool Start() abstract;
	virtual void CloseService()
	{
	}

	void Broadcast(SendBufferRef sendBuffer)
	{
		WRITE_LOCK;
		for (const auto& session : _sessions) {
			session->Send(sendBuffer);
		}
	}

	SessionRef CreateSession()
	{
		SessionRef session = _sessionFactory();
		session->SetService(shared_from_this());

		if (_iocpCore->Register(session) == false)
			return nullptr;

		return session;
	}

	void AddSession(SessionRef session)
	{
		WRITE_LOCK;
		_sessionCount++;
		_sessions.insert(session);
	}

	void ReleaseSession(SessionRef session)
	{
		WRITE_LOCK;
		ASSERT_CRASH(_sessions.erase(session) != 0);
		_sessionCount--;
	}
public:
	bool CanStart() { return _sessionFactory != nullptr; }
	int32 GetCurrentSessionCount() { return _sessionCount; }
	int32 GetMaxSessionCount() { return _maxSessionCount; }
	ServiceType GetServiceType() { return _type; }
	NetAddress GetNetAddress() { return _netAddress; }
	IOCPCoreRef& GetIocpCore() { return _iocpCore; }
	void SetSessionFactory(SessionFactory func) { _sessionFactory = func; }
protected:
	USE_LOCK;

	ServiceType _type;
	NetAddress _netAddress = {};
	IOCPCoreRef _iocpCore;

	Set<SessionRef> _sessions;
	int32 _sessionCount = 0;
	int32 _maxSessionCount = 0;
	SessionFactory _sessionFactory;
};

////////////////////////////////////////////

class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, IOCPCoreRef core, SessionFactory factory, int32 maxSessionCount = 1)
		: Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
	{
	}
	virtual ~ClientService() {}
public:
	virtual bool Start() override
	{
		if (CanStart() == false)
			return false;

		const int32 sessionCount = GetMaxSessionCount();
		for (int32 i = 0; i < sessionCount; ++i) {
			SessionRef session = CreateSession();
			if (session->Connect() == false)
				return false;
		}

		return true;
	}
};

////////////////////////////////////////////

class ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, IOCPCoreRef core, SessionFactory factory, int32 maxSessionCount = 1)
		: Service(ServiceType::Server, targetAddress, core, factory, maxSessionCount)
	{
	}
	virtual ~ServerService() {}
public:
	virtual bool Start() override
	{
		if (CanStart() == false)
			return false;

		_listener = MakeShared<Listener>();
		if (_listener == nullptr)
			return false;

		ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
		if (_listener->StartAccept(service) == false)
			return false;

		return true;
	}

	virtual void CloseService() override
	{
		Service::CloseService();
	}
private:
	ListenerRef _listener = nullptr;
};
