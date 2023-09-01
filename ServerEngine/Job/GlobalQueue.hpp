#pragma once

namespace Engine
{
	class GlobalQueue
	{
	public:
		GlobalQueue();
		~GlobalQueue();

		void					Push(JobQueueRef jobQueue);
		JobQueueRef				Pop();

	private:
		LockJobQueue<JobQueueRef> m_JobQueues;
	};
}