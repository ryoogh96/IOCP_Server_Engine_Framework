#include "pch.hpp"
#include "Room.hpp"
#include "Player.hpp"
#include "../GameSession.hpp"

Room GRoom;

void Room::Enter(PlayerRef player)
{
	WRITE_LOCK;
	m_Players[player->playerId] = player;
}

void Room::Leave(PlayerRef player)
{
	WRITE_LOCK;
	m_Players.erase(player->playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (auto& p : m_Players)
	{
		p.second->ownerSession->Send(sendBuffer);
	}
}