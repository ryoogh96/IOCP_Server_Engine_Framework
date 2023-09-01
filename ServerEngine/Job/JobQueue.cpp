#include "pch.hpp"
#include "JobQueue.hpp"


namespace Engine
{
	void JobQueue::Push(JobRef&& job)
	{
		const int32 prevCount = m_JobCount.fetch_add(1);
		m_Jobs.Push(job); // WRITE_LOCK

		// the first thread which pushed Job will doing execute.
		if (prevCount == 0)
		{
			Execute();
		}
	}

	// 1. what if there are too much Jobs to do.
	// 2. what if DoAsync not finish forever. (all Jobs are only belong to single thread)
	void JobQueue::Execute()
	{
		while (true)
		{
			Vector<JobRef> jobs;
			m_Jobs.PopAll(OUT jobs);

			const int32 jobCount = static_cast<int32>(jobs.size());
			for (int32 i = 0; i < jobCount; i++)
				jobs[i]->Execute();

			// if the remain job is zero, end execution.
			if (m_JobCount.fetch_sub(jobCount) == jobCount)
			{
				return;
			}
		}
	}

}