#include "pch.hpp"
#include "Room.hpp"
#include "Player.hpp"
#include "../Main/GameSession.hpp"

std::shared_ptr<Room> GRoom = make_shared<Room>();

void Room::Enter(PlayerRef player)
{
	m_Players[player->playerId] = player;
}

void Room::Leave(PlayerRef player)
{
	m_Players.erase(player->playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& p : m_Players)
	{
		p.second->ownerSession->Send(sendBuffer);
	}
}