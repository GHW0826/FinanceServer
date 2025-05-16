#pragma once

class GameSession;

using GameSessionRef = std::shared_ptr<GameSession>;
using IocpObjectRef = std::shared_ptr<IocpObject>;

class GameSessionManager
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);
private:
	USE_LOCK;
	Set<GameSessionRef> _sessions;
};

extern GameSessionManager GSessionManager;

