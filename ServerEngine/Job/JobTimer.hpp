#pragma once

namespace Engine
{
#pragma once

	struct JobData
	{
		JobData(std::weak_ptr<JobQueue> owner, JobRef job) : m_Owner(owner), m_Job(job)
		{

		}

		std::weak_ptr<JobQueue>	m_Owner;
		JobRef					m_Job;
	};

	struct TimerItem
	{
		bool operator<(const TimerItem& other) const
		{
			return executeTick > other.executeTick;
		}

		uint64 executeTick = 0;
		JobData* jobData = nullptr;
	};

	class JobTimer
	{
	public:
		void			Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, JobRef job);
		void			Distribute(uint64 now);
		void			Clear();

	private:
		USE_LOCK;
		PriorityQueue<TimerItem>	m_Items;
		Atomic<bool>				m_Distributing = false;
	};
}