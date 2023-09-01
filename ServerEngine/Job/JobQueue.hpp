#pragma once
#include "Job.hpp"
#include "Utility/LockJobQueue.hpp"

namespace Engine
{
	class JobQueue : public std::enable_shared_from_this<JobQueue>
	{
	public:
		void DoAsync(CallbackType&& callback)
		{
			Push(ObjectPool<Job>::MakeShared(std::move(callback)));
		}

		template<typename T, typename Ret, typename... Args>
		void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
		{
			std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
			Push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...));
		}

		void					ClearJobs() { m_Jobs.Clear(); }

	private:
		void					Push(JobRef&& job);

	public:
		void					Execute();

	protected:
		LockJobQueue<JobRef>	m_Jobs;
		Atomic<int32>			m_JobCount = 0;
	};
}