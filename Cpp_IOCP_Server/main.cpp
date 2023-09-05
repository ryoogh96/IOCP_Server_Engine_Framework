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
#include "DB/DBConnectionPool.hpp"

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

	{
		auto query = L"									\
			DROP TABLE IF EXISTS [dbo].[Gold];			\
			CREATE TABLE [dbo].[Gold]					\
			(											\
				[id] INT NOT NULL PRIMARY KEY IDENTITY, \
				[gold] INT NULL							\
			);";

		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}


	for (int32 i = 0; i < 3; i++)
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		dbConn->Unbind();

		int32 gold = 100;
		SQLLEN len = 0;

		ASSERT_CRASH(dbConn->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));


		ASSERT_CRASH(dbConn->Execute(L"INSERT INTO [dbo].[Gold]([gold]) VALUES(?)"));

		GDBConnectionPool->Push(dbConn);
	}


	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		dbConn->Unbind();

		int32 gold = 100;
		SQLLEN len = 0;

		ASSERT_CRASH(dbConn->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));

		int32 outId = 0;
		SQLLEN outIdLen = 0;
		ASSERT_CRASH(dbConn->BindCol(1, SQL_C_LONG, sizeof(outId), &outId, &outIdLen));

		int32 outGold = 0;
		SQLLEN outGoldLen = 0;
		ASSERT_CRASH(dbConn->BindCol(2, SQL_C_LONG, sizeof(outGold), &outGold, &outGoldLen));


		ASSERT_CRASH(dbConn->Execute(L"SELECT id, gold FROM [dbo].[Gold] WHERE gold = (?)"));

		while (dbConn->Fetch())
		{
			cout << "Id: " << outId << " Gold : " << outGold << endl;
		}

		GDBConnectionPool->Push(dbConn);
	}

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