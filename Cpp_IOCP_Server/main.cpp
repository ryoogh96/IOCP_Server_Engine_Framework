#include "pch.hpp"
#include "Thread/ThreadManager.hpp"
#include "Network/Service.hpp"
#include "Network/Session.hpp"
#include "Main/GameSession.hpp"
#include "Main/GameSessionManager.hpp"
#include "Utility/BufferWriter.hpp"
#include "Main/ClientPacketHandler.hpp"
#include "Protocol/Protocol.pb.h"
#include "GameContents/Room.hpp"
#include "Job/Job.hpp"
#include "DB/DBConnectionPool.hpp"
#include "DB/DBBind.hpp"
#include "XML/XmlParser.hpp"
#include "DB/DBSynchronizer.hpp"

using namespace std;
using namespace Engine;

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// process Network I/O -> to Ingame Logic (by Packet Handler)
		service->GetIOCPManager()->Dispatch(10);

		ThreadManager::DistributeReservedJobs();

		ThreadManager::DoGlobalQueueWork();
	}
}


int main()
{
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\ProjectsV13;Database=ServerDB;Trusted_Connection=Yes;"));

	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"Main/DB/GameDB.xml");

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

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}