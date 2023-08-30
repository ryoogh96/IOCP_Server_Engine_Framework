#include "pch.hpp"
#include "GameSession.hpp"
#include "GameSessionManager.hpp"

void GameSession::OnConnected()
{
	GSessionManager.Add(std::static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(std::static_pointer_cast<GameSession>(shared_from_this()));
}

Engine::int32 GameSession::OnRecvPacket(BYTE* buffer, Engine::int32 len)
{
	Engine::PacketHeader header = *(reinterpret_cast<Engine::PacketHeader*>(buffer));
	std::cout << "Packet ID : " << header.id << "Size : " << header.size << std::endl;

	return len;
}

void GameSession::OnSend(Engine::int32 len)
{

}