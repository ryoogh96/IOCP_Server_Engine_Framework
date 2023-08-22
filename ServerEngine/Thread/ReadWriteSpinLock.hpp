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
		void ReadLock(const char* name);
		void ReadUnlock(const char* name);
		void WriteLock(const char* name);
		void WriteUnlock(const char* name);

	private:
		Atomic<uint32> m_LockFlag = EMPTY_FLAG;
		uint16 m_WriteCount = 0;
	};

	class ReadLockGuard
	{
	public:
		ReadLockGuard(ReadWriteSpinLock& lock, const char* name) : m_Lock(lock), m_Name(name) { m_Lock.ReadLock(name); }
		~ReadLockGuard() { m_Lock.ReadUnlock(m_Name); }

	private:
		ReadWriteSpinLock& m_Lock;
		const char* m_Name;
	};

	class WriteLockGuard
	{
	public:
		WriteLockGuard(ReadWriteSpinLock& lock, const char* name) : m_Lock(lock), m_Name(name){ m_Lock.WriteLock(name); }
		~WriteLockGuard() { m_Lock.WriteUnlock(m_Name); }

	private:
		ReadWriteSpinLock& m_Lock;
		const char* m_Name;
	};
}