#pragma once
#include "Job/JobQueue.hpp"

using namespace std;
using namespace Engine;

class Room : public JobQueue
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

private:
	map<uint64, PlayerRef> m_Players;
};

extern shared_ptr<Room> GRoom;