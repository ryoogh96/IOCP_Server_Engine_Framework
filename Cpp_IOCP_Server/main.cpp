#include "pch.hpp"
#include "Thread/ThreadManager.hpp"
#include "Network/Service.hpp"
#include "Network/Session.hpp"
#include "GameSession.hpp"
#include "GameSessionManager.hpp"

using namespace Engine;

int main()
{
    Engine::MiniDump dump;
    dump.BeginDump();

	Engine::ServerServiceRef service = Engine::MakeShared<Engine::ServerService>(
		Engine::NetAddress(L"127.0.0.1", 7777),
		Engine::MakeShared<Engine::IOCPManager>(),
		Engine::MakeShared<GameSession>,
		100);

	ASSERT_CRASH(service->Start());

	for (Engine::int32 i = 0; i < 5; i++)
	{
		Engine::GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIOCPManager()->Dispatch();
				}
			});
	}

	char sendData[1000] = "Hello World";

	while (true)
	{
		Engine::SendBufferRef sendBuffer = Engine::GSendBufferManager->Open(4096);

		BYTE* buffer = sendBuffer->Buffer();
		(reinterpret_cast<Engine::PacketHeader*>(buffer))->size = (sizeof(sendData) + sizeof(Engine::PacketHeader));
		(reinterpret_cast<Engine::PacketHeader*>(buffer))->id = 1; // 1 : Hello Msg
		::memcpy(&buffer[4], sendData, sizeof(sendData));
		sendBuffer->Close((sizeof(sendData) + sizeof(Engine::PacketHeader)));

		GSessionManager.Broadcast(sendBuffer);

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	Engine::GThreadManager->Join();

    return 0;
}