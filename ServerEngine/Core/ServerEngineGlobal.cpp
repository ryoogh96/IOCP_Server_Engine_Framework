#include "pch.hpp"
#include "ServerEngineGlobal.hpp"
#include "Memory/Memory.hpp"
#include "Job/GlobalQueue.hpp"
#include "Job/JobTimer.hpp"

namespace Engine
{
	ThreadManager*		GThreadManager = nullptr;
	Memory*				GMemory = nullptr;
	SendBufferManager*	GSendBufferManager = nullptr;
	GlobalQueue* GGlobalQueue = nullptr;
	JobTimer* GJobTimer = nullptr;

	DeadLockProfiler*	GDeadLockProfiler = nullptr;

	class ServerEngineGlobal
	{
	public:
		ServerEngineGlobal()
		{
			GThreadManager = new ThreadManager();
			GMemory = new Memory();
			GSendBufferManager = new SendBufferManager();
			GGlobalQueue = new GlobalQueue();
			GJobTimer = new JobTimer();
			GDeadLockProfiler = new DeadLockProfiler();
			SocketManager::Initialize();
		}

		~ServerEngineGlobal()
		{
			delete GThreadManager;
			delete GMemory;
			delete GSendBufferManager;
			delete GGlobalQueue;
			delete GJobTimer;
			delete GDeadLockProfiler;
			SocketManager::Clear();
		}
	} GServerEngineGlobal;
}