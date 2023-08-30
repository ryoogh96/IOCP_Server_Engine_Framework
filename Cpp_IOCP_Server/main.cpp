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
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

		BufferWriter bw(sendBuffer->Buffer(), 4096);

		PacketHeader* header = bw.Reserve<PacketHeader>();
		// id(uint64), hp(uint32), atk(uint16)
		bw << (uint64)1001 << (uint32)100 << (uint16)10;
		bw.Write(sendData, sizeof(sendData));

		header->size = bw.WriteSize();
		header->id = 1; // 1 : Test Msg

		sendBuffer->Close(bw.WriteSize());

		GSessionManager.Broadcast(sendBuffer);

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	Engine::GThreadManager->Join();

    return 0;
}