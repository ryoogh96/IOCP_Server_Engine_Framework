#pragma once

namespace Engine
{
	class JobQueue
	{
	public:
		void Push(JobRef job)
		{
			WRITE_LOCK;
			m_Jobs.push(job);
		}

		JobRef Pop()
		{
			WRITE_LOCK;
			if (m_Jobs.empty())
				return nullptr;

			JobRef ret = m_Jobs.front();
			m_Jobs.pop();
			return ret;
		}

	private:
		USE_LOCK;
		std::queue<JobRef> m_Jobs;
	};
}