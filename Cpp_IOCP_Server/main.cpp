#include "pch.hpp"
#include "Thread/ThreadManager.hpp"
#include "Network/Service.hpp"
#include "Network/Session.hpp"
#include "GameSession.hpp"
#include "GameSessionManager.hpp"
#include "Utility/BufferWriter.hpp"
#include "ServerPacketHandler.hpp"
#include "Protocol/Protocol.pb.h"

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
		Protocol::S_TEST pkt;
		pkt.set_id(1000);
		pkt.set_hp(100);
		pkt.set_attack(10);
		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(100);
			data->set_remaintime(1.2f);
			data->add_victims(4000);
		}
		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(200);
			data->set_remaintime(2.5f);
			data->add_victims(1000);
			data->add_victims(2000);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		GSessionManager.Broadcast(sendBuffer);

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	Engine::GThreadManager->Join();

    return 0;
}