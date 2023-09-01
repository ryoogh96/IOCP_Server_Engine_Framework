#pragma once
#include "Job.hpp"

using namespace std;
using namespace Engine;

class Room
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	void FlushJob();

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::* memFunc)(Args...), Args... args)
	{
		auto job = MakeShared<MemberJob<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args...);
		m_Jobs.Push(job);
	}

private:
	map<uint64, PlayerRef> m_Players;

	JobQueue m_Jobs;
};

extern Room GRoom;