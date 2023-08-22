#include "pch.hpp"
#include "ServerEngineGlobal.hpp"
#include "Memory/Memory.hpp"

namespace Engine
{
	ThreadManager* GThreadManager = nullptr;
	DeadLockProfiler* GDeadLockProfiler = nullptr;
	Memory* GMemory = nullptr;

	class ServerEngineGlobal
	{
	public:
		ServerEngineGlobal()
		{
			GThreadManager = new ThreadManager();
			GDeadLockProfiler = new DeadLockProfiler();
			GMemory = new Memory();
		}

		~ServerEngineGlobal()
		{
			delete GThreadManager;
			delete GDeadLockProfiler;
			delete GMemory;
		}
	} GServerEngineGlobal;
}