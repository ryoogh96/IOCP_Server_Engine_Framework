#include "pch.hpp"
#include "GameSessionManager.hpp"
#include "GameSession.hpp"

GameSessionManager GSessionManager;

void GameSessionManager::Add(GameSessionRef session)
{
	Engine::WRITE_LOCK;
	_sessions.insert(session);
}

void GameSessionManager::Remove(GameSessionRef session)
{
	Engine::WRITE_LOCK;
	_sessions.erase(session);
}

void GameSessionManager::Broadcast(Engine::SendBufferRef sendBuffer)
{
	Engine::WRITE_LOCK;
	for (GameSessionRef session : _sessions)
	{
		session->Send(sendBuffer);
	}
}