#pragma once

namespace Engine
{
	/*--------------------------------------------
	[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
	W : WriteFlag (Exclusive Lock Owner ThreadId)
	R : ReadFlag (Shared Lock Count)
	---------------------------------------------*/
	class ReadWriteSpinLock
	{
		enum : uint32
		{
			ACQUIRE_TIMEOUT_TICK = 10000,
			MAX_SPIN_COUNT = 5000,
			WRITE_THREAD_MASK = 0xFFFF'0000,
			READ_COUNT_MASK = 0x0000'FFFF,
			EMPTY_FLAG = 0x0000'0000
		};

	public:
		void ReadLock();
		void ReadUnlock();
		void WriteLock();
		void WriteUnlock();

	private:
		Atomic<uint32> m_LockFlag = EMPTY_FLAG;
		uint16 m_WriteCount = 0;
	};

	class ReadLockGuard
	{
	public:
		ReadLockGuard(ReadWriteSpinLock& lock) : m_Lock(lock) { m_Lock.ReadLock(); }
		~ReadLockGuard() { m_Lock.ReadUnlock(); }

	private:
		ReadWriteSpinLock& m_Lock;
	};

	class WriteLockGuard
	{
	public:
		WriteLockGuard(ReadWriteSpinLock& lock) : m_Lock(lock) { m_Lock.WriteLock(); }
		~WriteLockGuard() { m_Lock.WriteUnlock(); }

	private:
		ReadWriteSpinLock& m_Lock;
	};
}