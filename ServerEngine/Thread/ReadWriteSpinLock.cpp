#include "pch.hpp"
#include "ReadWriteSpinLock.hpp"

namespace Engine {
	void ReadWriteSpinLock::ReadLock()
	{
		// if the same thread own this, process success
		const uint32 lockThreadId = (m_LockFlag.load() & WRITE_THREAD_MASK) >> 16;
		if (LThreadId == lockThreadId)
		{
			m_LockFlag.fetch_add(1);
			return;
		}

		// if no one own this, race each other to increase shared lock count
		const int64 beginTick = ::GetTickCount64();
		while (true)
		{
			for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
			{
				uint32 expected = (m_LockFlag.load() & READ_COUNT_MASK);
				if (m_LockFlag.compare_exchange_strong(OUT expected, expected + 1))
					return;
			}

			if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
				CRASH("LOCK_TIMEOUT");

			std::this_thread::yield();
		}
	}

	void ReadWriteSpinLock::ReadUnlock()
	{
		if ((m_LockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
			CRASH("MULTIPLE_UNLOCK");
	}

	void ReadWriteSpinLock::WriteLock()
	{
		// if the same thread own this, process success
		const uint32 lockThreadId = (m_LockFlag.load() & WRITE_THREAD_MASK) >> 16;
		if (LThreadId == lockThreadId)
		{
			m_WriteCount++;
			return;
		}

		// if no one own this, race each other to acquire writeLock
		const int64 beginTick = ::GetTickCount64();
		const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
		while (true)
		{
			for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
			{
				uint32 expected = EMPTY_FLAG;
				if (m_LockFlag.compare_exchange_strong(OUT expected, desired))
				{
					m_WriteCount++;
					return;
				}
			}

			if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
				CRASH("LOCK_TIMEOUT");

			std::this_thread::yield();
		}
	}

	void ReadWriteSpinLock::WriteUnlock()
	{
		// before to release all ReadLock, make impossible to acquire WriteLock
		if ((m_LockFlag.load() & READ_COUNT_MASK) != 0)
			CRASH("INVALID_UNLOCK_ORDER");
	
		const int32 lockCount = --m_WriteCount;
		if (lockCount == 0)
			m_LockFlag.store(EMPTY_FLAG);
	}
}