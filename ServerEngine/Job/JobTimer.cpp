#include "pch.hpp"
#include "JobTimer.hpp"
#include "JobQueue.hpp"

namespace Engine
{
	void JobTimer::Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, JobRef job)
	{
		const uint64 executeTick = ::GetTickCount64() + tickAfter;
		JobData* jobData = ObjectPool<JobData>::Pop(owner, job);

		WRITE_LOCK;

		m_Items.push(TimerItem{ executeTick, jobData });
	}

	void JobTimer::Distribute(uint64 now)
	{
		// only single thread can execute once
		if (m_Distributing.exchange(true) == true)
			return;

		Vector<TimerItem> items;

		{
			WRITE_LOCK;

			while (m_Items.empty() == false)
			{
				const TimerItem& timerItem = m_Items.top();
				if (now < timerItem.executeTick)
					break;

				items.push_back(timerItem);
				m_Items.pop();
			}
		}

		for (TimerItem& item : items)
		{
			if (JobQueueRef owner = item.jobData->m_Owner.lock())
				owner->Push(item.jobData->m_Job);

			ObjectPool<JobData>::Push(item.jobData);
		}

		m_Distributing.store(false);
	}

	void JobTimer::Clear()
	{
		WRITE_LOCK;

		while (m_Items.empty() == false)
		{
			const TimerItem& timerItem = m_Items.top();
			ObjectPool<JobData>::Push(timerItem.jobData);
			m_Items.pop();
		}
	}

}