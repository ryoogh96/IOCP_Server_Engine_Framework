#pragma once

class GameSession;

using GameSessionRef = std::shared_ptr<GameSession>;

class GameSessionManager
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(Engine::SendBufferRef sendBuffer);

private:
	Engine::USE_LOCK;
	Engine::Set<GameSessionRef> _sessions;
};

extern GameSessionManager GSessionManager;
