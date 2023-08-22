#include "pch.hpp"
#include "ServerEngineGlobal.hpp"
#include "Memory/Memory.hpp"

namespace Engine
{
	ThreadManager* GThreadManager = nullptr;
	Memory* GMemory = nullptr;

	class ServerEngineGlobal
	{
	public:
		ServerEngineGlobal()
		{
			GThreadManager = new ThreadManager();
			GMemory = new Memory();
		}

		~ServerEngineGlobal()
		{
			delete GThreadManager;
			delete GMemory;
		}
	} GServerEngineGlobal;
}