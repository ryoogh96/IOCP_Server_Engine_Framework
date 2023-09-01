#include "pch.hpp"
#include "GlobalQueue.hpp"

namespace Engine
{
	GlobalQueue::GlobalQueue()
	{

	}

	GlobalQueue::~GlobalQueue()
	{

	}

	void GlobalQueue::Push(JobQueueRef jobQueue)
	{
		m_JobQueues.Push(jobQueue);
	}

	JobQueueRef GlobalQueue::Pop()
	{
		return m_JobQueues.Pop();
	}
}