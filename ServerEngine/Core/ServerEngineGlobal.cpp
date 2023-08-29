#include "pch.hpp"
#include "ServerEngineGlobal.hpp"
#include "Memory/Memory.hpp"

namespace Engine
{
	ThreadManager*		GThreadManager = nullptr;
	Memory*				GMemory = nullptr;
	SendBufferManager*	GSendBufferManager = nullptr;

	DeadLockProfiler*	GDeadLockProfiler = nullptr;

	class ServerEngineGlobal
	{
	public:
		ServerEngineGlobal()
		{
			GThreadManager = new ThreadManager();
			GMemory = new Memory();
			GSendBufferManager = new SendBufferManager();
			GDeadLockProfiler = new DeadLockProfiler();
		}

		~ServerEngineGlobal()
		{
			delete GThreadManager;
			delete GMemory;
			delete GSendBufferManager;
			delete GDeadLockProfiler;
		}
	} GServerEngineGlobal;
}