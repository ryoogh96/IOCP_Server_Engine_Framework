#include "pch.hpp"
#include "ServerEngineGlobal.hpp"
#include "Memory/Memory.hpp"

namespace Engine
{
	Memory* GMemory = nullptr;

	class ServerEngineGlobal
	{
	public:
		ServerEngineGlobal()
		{
			GMemory = new Memory();
		}

		~ServerEngineGlobal()
		{
			delete GMemory;
		}
	};
}