#pragma once
#include "Job.hpp"
#include "JobQueue.hpp"

namespace Engine
{
	class JobSerializer : public std::enable_shared_from_this<JobSerializer>
	{
	public:
		void PushJob(CallbackType&& callback)
		{
			auto job = ObjectPool<Job>::MakeShared(std::move(callback));
			m_JobQueue.Push(job);
		}

		template<typename T, typename Ret, typename... Args>
		void PushJob(Ret(T::* memFunc)(Args...), Args... args)
		{
			std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
			auto job = ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
			m_JobQueue.Push(job);
		}

		virtual void FlushJob() abstract;

	protected:
		JobQueue m_JobQueue;
	};
}