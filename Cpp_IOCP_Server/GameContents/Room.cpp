#include "pch.hpp"
#include "Room.hpp"
#include "Player.hpp"
#include "../GameSession.hpp"

Room GRoom;

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

void Room::FlushJob()
{
	while (true)
	{
		JobRef job = m_Jobs.Pop();
		if (job == nullptr)
			break;

		job->Execute();
	}
}
