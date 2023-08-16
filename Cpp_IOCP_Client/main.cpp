#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.hpp"

#include "ServerEnginePCH.hpp"

int main()
{
	Engine::MiniDump minidump;
	minidump.BeginDump();

	Engine::ClientSocketManager*clientSocketManager = new Engine::ClientSocketManager();


	char recvBuffer[1024] = "client msg";
	while (true)
	{
		clientSocketManager->Send(recvBuffer);

		clientSocketManager->Recv();

		std::this_thread::sleep_for(std::chrono::seconds(1));

		//clientSocketManager->SetRecvBuffer(recvBuffer);
	}

	delete clientSocketManager;

	return 0;
}
