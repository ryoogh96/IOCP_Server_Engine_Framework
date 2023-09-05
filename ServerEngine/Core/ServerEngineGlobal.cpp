#include "pch.hpp"
#include "ServerEngineGlobal.hpp"
#include "Memory/Memory.hpp"
#include "Job/GlobalQueue.hpp"
#include "Job/JobTimer.hpp"
#include "DB/DBConnectionPool.hpp"
#include "ConsoleLog/ConsoleLog.hpp"

namespace Engine
{
	ThreadManager*		GThreadManager = nullptr;
	Memory*				GMemory = nullptr;
	SendBufferManager*	GSendBufferManager = nullptr;
	GlobalQueue* GGlobalQueue = nullptr;
	JobTimer* GJobTimer = nullptr;

	DeadLockProfiler*	GDeadLockProfiler = nullptr;
	DBConnectionPool* GDBConnectionPool = nullptr;
	ConsoleLog* GConsoleLogger = nullptr;

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
			GDBConnectionPool = new DBConnectionPool();
			GConsoleLogger = new ConsoleLog();
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
			delete GDBConnectionPool;
			delete GConsoleLogger;
			SocketManager::Clear();
		}
	} GServerEngineGlobal;
}