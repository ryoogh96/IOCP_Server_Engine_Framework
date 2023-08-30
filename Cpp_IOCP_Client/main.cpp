#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.hpp"

#include "Core/ServerEnginePCH.hpp"

int main()
{
	Engine::MiniDump minidump;
	minidump.BeginDump();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}
