#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.hpp"

#include "ServerEnginePCH.hpp"

int main()
{
	Engine::MiniDump minidump;
	minidump.beginDump();

	Engine::ClientSocketManager*clientSocketManager = new Engine::ClientSocketManager();


	char recvBuffer[1024] = "client msg";
	while (true)
	{
		clientSocketManager->recv();

		this_thread::sleep_for(std::chrono::seconds(1));

		clientSocketManager->send(clientSocketManager->getRecvBuffer());

		clientSocketManager->setRecvBuffer(recvBuffer);
	}

	delete clientSocketManager;

	return 0;
}
