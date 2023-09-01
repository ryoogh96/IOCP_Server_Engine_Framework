#include "pch.hpp"
#include "Thread/ThreadManager.hpp"
#include "Network/Service.hpp"
#include "Network/Session.hpp"
#include "GameSession.hpp"
#include "GameSessionManager.hpp"
#include "Utility/BufferWriter.hpp"
#include "ClientPacketHandler.hpp"
#include "Protocol/Protocol.pb.h"
#include "GameContents/Room.hpp"
#include "Job/Job.hpp"

using namespace std;
using namespace Engine;

int main()
{
	ClientPacketHandler::Initialize();

	ServerServiceRef service = MakeShared<Engine::ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IOCPManager>(),
		MakeShared<GameSession>,
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIOCPManager()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}