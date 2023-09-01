#pragma once
#include "Job/JobSerializer.hpp"

using namespace std;
using namespace Engine;

class Room : public JobSerializer
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	virtual void FlushJob() override;

private:
	map<uint64, PlayerRef> m_Players;
};

extern shared_ptr<Room> GRoom;