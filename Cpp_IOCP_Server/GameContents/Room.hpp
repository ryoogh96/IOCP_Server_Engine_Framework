#pragma once

using namespace Engine;

class Room
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	std::map<uint64, PlayerRef> m_Players;
};

extern Room GRoom;