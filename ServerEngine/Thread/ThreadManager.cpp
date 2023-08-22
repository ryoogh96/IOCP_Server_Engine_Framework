#include "pch.hpp"
#include "ThreadManager.hpp"

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
}