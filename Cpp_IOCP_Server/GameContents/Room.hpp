#pragma once
#include "Job.hpp"

using namespace std;
using namespace Engine;

class Room
{
	friend class EnterJob;
	friend class LeaveJob;
	friend class BroadcastJob;

private:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	void PushJob(JobRef job) { m_Jobs.Push(job); }
	void FlushJob();

private:
	map<uint64, PlayerRef> m_Players;

	JobQueue m_Jobs;
};

extern Room GRoom;

class EnterJob : public IJob
{
public:
	EnterJob(Room& room, PlayerRef player) : m_Room(room), m_Player(player)
	{
	}

	virtual void Execute() override
	{
		m_Room.Enter(m_Player);
	}

public:
	Room& m_Room;
	PlayerRef m_Player;
};

class LeaveJob : public IJob
{
public:
	LeaveJob(Room& room, PlayerRef player) : m_Room(room), m_Player(player)
	{
	}

	virtual void Execute() override
	{
		m_Room.Leave(m_Player);
	}

public:
	Room& m_Room;
	PlayerRef m_Player;
};

class BroadcastJob : public IJob
{
public:
	BroadcastJob(Room& room, SendBufferRef sendBuffer) : m_Room(room), m_SendBuffer(sendBuffer)
	{
	}

	virtual void Execute() override
	{
		m_Room.Broadcast(m_SendBuffer);
	}

public:
	Room& m_Room;
	SendBufferRef m_SendBuffer;
};