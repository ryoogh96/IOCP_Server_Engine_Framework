#include "pch.hpp"
#include "GameSession.hpp"
#include "GameSessionManager.hpp"
#include "ClientPacketHandler.hpp"
#include "../GameContents/Room.hpp"

void GameSession::OnConnected()
{
	GSessionManager.Add(std::static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(std::static_pointer_cast<GameSession>(shared_from_this()));

	if (m_CurrentPlayer)
	{
		if (auto room = m_Room.lock())
			room->DoAsync(&Room::Leave, m_CurrentPlayer);
	}

	m_CurrentPlayer = nullptr;
	m_Players.clear();
}

void GameSession::OnRecvPacket(BYTE* buffer, Engine::int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(Engine::int32 len)
{

}