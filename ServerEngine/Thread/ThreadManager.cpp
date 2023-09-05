#include "pch.hpp"
#include "ThreadManager.hpp"
#include "Core/ServerEngineTLS.hpp"
#include "Core/ServerEngineGlobal.hpp"
#include "Job/GlobalQueue.hpp"

namespace Engine
{
	ThreadManager::ThreadManager()
	{
		// main thread
		InitTLS();
	}

	ThreadManager::~ThreadManager()
	{
		Join();
	}

	void ThreadManager::Launch(std::function<void(void)> callback)
	{
		LockGuard guard(m_Lock);
		
		m_Threads.push_back(std::thread([=]()
			{
				InitTLS();
				callback();
				DestoryTLS();
			}));
	}

	void ThreadManager::Join()
	{
		for (std::thread& thread : m_Threads)
		{
			if (thread.joinable())
				thread.join();
		}
		m_Threads.clear();
	}

	void ThreadManager::InitTLS()
	{
		static Atomic<uint32> SThreadId = 1;
		LThreadId = SThreadId.fetch_add(1);
	}

	void ThreadManager::DestoryTLS()
	{

	}

	void ThreadManager::DoGlobalQueueWork()
	{
		while (true)
		{
			uint64 now = ::GetTickCount64();
			if (now > LEndTickCount)
				break;

			JobQueueRef jobQueue = GGlobalQueue->Pop();
			if (jobQueue == nullptr)
				break;

			jobQueue->Execute();
		}
	}

	void ThreadManager::DistributeReservedJobs()
	{
		const uint64 now = ::GetTickCount64();

		GJobTimer->Distribute(now);
	}
}