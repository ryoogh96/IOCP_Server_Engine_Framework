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
#include "GameContents/Job.hpp"

using namespace std;
using namespace Engine;

void HealByValue(int64 target, int32 value)
{
	cout << target << "gives " << value << " heals" << endl;
}

class Knight
{
public:
	void HealMe(int32 value)
	{
		cout << "HealMe! " << value << endl;
	}
};

int main()
{
    Engine::MiniDump dump;
    dump.BeginDump();

	auto tup = std::tuple<int32, int32>(1, 2);
	auto val0 = std::get<0>(tup);
	auto val1 = std::get<1>(tup);

	auto s = gen_seq<3>();
	// gen_seq<3>
	// : gen_seq<2, 2>
	// : gen_seq<1, 1, 2>
	// : gen_seq<0, 0, 1, 2>
	// : seq<0, 1, 2>

	// TEST JOB
	{
		FuncJob<void, int64, int32> job(HealByValue, 100, 10);
		job.Execute();
	}
	{
		Knight k1;
		MemberJob job2(&k1, &Knight::HealMe, 10);
		job2.Execute();
	}

	// JOB

	ClientPacketHandler::Initialize();

	ServerServiceRef service = MakeShared<Engine::ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<Engine::IOCPManager>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
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

	while (true)
	{
		GRoom.FlushJob();
		this_thread::sleep_for(1ms);
	}

	GThreadManager->Join();
}