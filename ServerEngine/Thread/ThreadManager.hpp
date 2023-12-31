#pragma once

#include <thread>
#include <functional>

namespace Engine
{
	class ThreadManager
	{
	public:
		ThreadManager();
		~ThreadManager();

		void	Launch(std::function<void(void)> callback);
		void	Join();

		static void InitTLS();
		static void DestoryTLS();

		static void DoGlobalQueueWork();
		static void DistributeReservedJobs();

	private:
		Mutex						m_Lock;
		std::vector<std::thread>	m_Threads;
	};
}