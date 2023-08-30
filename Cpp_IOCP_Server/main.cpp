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
		vector<BuffData> buffs{ BuffData {100, 1.5f}, BuffData{200, 2.3f}, BuffData {300, 0.7f } };
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_TEST(1001, 100, 10, buffs, L"안녕하세요");
		GSessionManager.Broadcast(sendBuffer);

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	Engine::GThreadManager->Join();

    return 0;
}