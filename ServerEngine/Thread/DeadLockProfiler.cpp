#include "pch.hpp"
#include "DeadLockProfiler.hpp"

namespace Engine
{
	void DeadLockProfiler::PushLock(const char* name)
	{
		LockGuard guard(m_Lock);

		int32 lockId = 0;

		auto findIt = m_NameToId.find(name);
		if (findIt == m_NameToId.end())
		{
			lockId = static_cast<int32>(m_NameToId.size());
			m_NameToId[name] = lockId;
			m_IdToName[lockId] = name;
		}
		else
		{
			lockId = findIt->second;
		}

		// if already locked
		if (LLockStack.empty() == false)
		{
			// in case of undiscovered previously, check the deadlock
			const int32 prevId = LLockStack.top();
			if (lockId != prevId)
			{
				std::set<int32>& history = m_LockHistory[prevId];
				if (history.find(lockId) == history.end())
				{
					history.insert(lockId);
					CheckCycle();
				}
			}
		}

		LLockStack.push(lockId);
	}

	void DeadLockProfiler::PopLock(const char* name)
	{
		LockGuard guard(m_Lock);

		if (LLockStack.empty())
			CRASH("MULTIPLE_UNLOCK");

		int32 lockId = m_NameToId[name];
		if (LLockStack.top() != lockId)
			CRASH("INVALID_UNLOCK");

		LLockStack.pop();
	}

	void DeadLockProfiler::CheckCycle()
	{
		const int32 lockCount = static_cast<int32>(m_NameToId.size());
		m_DiscoveredOrder = std::vector<int32>(lockCount, -1);
		m_DiscoveredCount = 0;
		m_Finished = std::vector<bool>(lockCount, false);
		m_Parent = std::vector<int32>(lockCount, -1);

		for (int32 lockId = 0; lockId < lockCount; lockId++)
			DFS(lockId);

		m_DiscoveredOrder.clear();
		m_Finished.clear();
		m_Parent.clear();
	}

	void DeadLockProfiler::DFS(int32 here)
	{
		if (m_DiscoveredOrder[here] != -1)
			return;

		m_DiscoveredOrder[here] = m_DiscoveredCount++;

		// traverse all adjacent vertexes.
		auto findIt = m_LockHistory.find(here);
		if (findIt == m_LockHistory.end())
		{
			m_Finished[here] = true;
			return;
		}

		std::set<int32>& nextSet = findIt->second;
		for (int32 there : nextSet)
		{
			// if not discovered yet, visit it.
			if (m_DiscoveredOrder[there] == -1)
			{
				m_Parent[there] = here;
				DFS(there);
				continue;
			}

			// if "here" has been found before "there", "there" is a parent of "here" (Forward Edge)
			if (m_DiscoveredOrder[here] < m_DiscoveredOrder[there])
				continue;

			// if this is not forward edge, and DFS("there") is not finished, "there" is a parent of "here" (Backward Edge)
			if (m_Finished[there] == false)
			{
				std::cout << m_IdToName[here] << " -> " << m_IdToName[there] << std::endl;

				int32 now = here;
				while (true)
				{
					std::cout << m_IdToName[m_Parent[now]] << " -> " << m_IdToName[now] << std::endl;
					now = m_Parent[now];
					if (now == there)
						break;
				}

				CRASH("DEADLOCK_DETECTED");
			}
		}

		m_Finished[here] = true;
	}
}