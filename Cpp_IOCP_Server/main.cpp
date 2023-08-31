#include "pch.hpp"
#include "Thread/ThreadManager.hpp"
#include "Network/Service.hpp"
#include "Network/Session.hpp"
#include "GameSession.hpp"
#include "GameSessionManager.hpp"
#include "Utility/BufferWriter.hpp"
#include "ServerPacketHandler.hpp"

using namespace std;
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

	while (true)
	{
		// [ PKT_S_TEST ]
		PKT_S_TEST_WRITE pktWriter(1001, 100, 10);

		// [ PKT_S_TEST ][BuffsListItem BuffsListItem BuffsListItem]
		PKT_S_TEST_WRITE::BuffsList buffList = pktWriter.ReserveBuffsList(3);
		buffList[0] = { 100, 1.5f };
		buffList[1] = { 200, 2.3f };
		buffList[2] = { 300, 0.7f };

		PKT_S_TEST_WRITE::BuffsVictimsList vic0 = pktWriter.ReserveBuffsVictimsList(&buffList[0], 3);
		{
			vic0[0] = 1000;
			vic0[1] = 2000;
			vic0[2] = 3000;
		}

		PKT_S_TEST_WRITE::BuffsVictimsList vic1 = pktWriter.ReserveBuffsVictimsList(&buffList[1], 1);
		{
			vic1[0] = 1000;
		}

		PKT_S_TEST_WRITE::BuffsVictimsList vic2 = pktWriter.ReserveBuffsVictimsList(&buffList[2], 2);
		{
			vic2[0] = 3000;
			vic2[1] = 5000;
		}

		SendBufferRef sendBuffer = pktWriter.CloseAndReturn();

		GSessionManager.Broadcast(sendBuffer);

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	Engine::GThreadManager->Join();

    return 0;
}