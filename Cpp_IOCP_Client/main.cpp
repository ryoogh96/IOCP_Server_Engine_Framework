#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.hpp"

#include "Core/ServerEnginePCH.hpp"

//class ClientSession : public Engine::PacketSession
//{
//public:
//	ClientSession() { std::cout << "ClientSession()" << std::endl; }
//	~ClientSession() { std::cout << "~ClientSession()" << std::endl; }
//
//	virtual Engine::int32 OnRecvPacket(BYTE* buffer, Engine::int32 len) override
//	{
//		Engine::PacketHeader header = *(reinterpret_cast<Engine::PacketHeader*>(buffer[0]));
//		
//		std::cout << "PAcket ID : " << header.id << "Size: " << header.size << std::endl;
//
//		char recvBuffer[4096];
//		::memcpy(recvBuffer, &buffer[4], header.size - sizeof(Engine::PacketHeader));
//
//		std::cout << "recvBuffer: " << recvBuffer << std::endl;
//
//		return len;
//	}
//};

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
