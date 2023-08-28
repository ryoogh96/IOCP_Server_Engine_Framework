#pragma once

namespace Engine
{
	class DeadLockProfiler
	{
	public:
		void PushLock(const char* name);
		void PopLock(const char* name);
		void CheckCycle();

	private:
		void DFS(int32 index);

	private:
		std::unordered_map<const char*, int32>	m_NameToId;
		std::unordered_map<int32, const char*>	m_IdToName;
		std::map<int32, std::set<int32>>		m_LockHistory;

		Mutex m_Lock;

	private:
		std::vector<int32>	m_DiscoveredOrder;
		int32				m_DiscoveredCount = 0;
		std::vector<bool>	m_Finished;
		std::vector<int32>	m_Parent;
	};
}