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
		clientSocketManager->Recv();

		this_thread::sleep_for(std::chrono::seconds(1));

		clientSocketManager->Send(clientSocketManager->GetRecvBuffer());

		clientSocketManager->SetRecvBuffer(recvBuffer);
	}

	delete clientSocketManager;

	return 0;
}
