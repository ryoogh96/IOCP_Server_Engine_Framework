#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.hpp"

#include "ServerEnginePCH.hpp"

// enum class OP {
//	OP_SEND_MSG,
//	OP_ACCEPT_CLIENT,
// };
//
// struct sc_packet_chat {
//	WSAOVERLAPPED overlapped = {};
//	char size;
//	char type;
//	int	 id;
//	wchar_t message[100];
// };
//
// struct cs_packet_chat {
//	WSAOVERLAPPED overlapped = {};
//	OP type;
//	char size;
//	int id;
//	std::string message;
// };

int main()
{
	Engine::MiniDump minidump;
	minidump.beginDump();

	Engine::ClientSocketManager*clientSocketManager = new Engine::ClientSocketManager();


	char recvBuffer[1024] = "client msg";
	while (true)
	{
		//char sendBuffer[100] = "Hello World";
		//clientSocketManager->send(sendBuffer);

		clientSocketManager->recv();

		Sleep(1000);

		clientSocketManager->send(clientSocketManager->getRecvBuffer());

		clientSocketManager->setRecvBuffer(recvBuffer);
	}

	delete clientSocketManager;

	return 0;
}
