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
	XmlNode root;
	XmlParser parser;
	if (parser.ParseFromFile(L"Main/DB/GameDB.xml", OUT root) == false)
		return 0;

	Vector<XmlNode> tables = root.FindChildren(L"Table");
	for (XmlNode& table : tables)
	{
		String name = table.GetStringAttr(L"name");
		String desc = table.GetStringAttr(L"desc");

		Vector<XmlNode> columns = table.FindChildren(L"Column");
		for (XmlNode& column : columns)
		{
			String colName = column.GetStringAttr(L"name");
			String colType = column.GetStringAttr(L"type");
			bool nullable = !column.GetBoolAttr(L"notnull", false);
			String identity = column.GetStringAttr(L"identity");
			String colDefault = column.GetStringAttr(L"default");
		}

		Vector<XmlNode> indices = table.FindChildren(L"Index");
		for (XmlNode& index : indices)
		{
			String indexType = index.GetStringAttr(L"type");
			bool primaryKey = index.FindChild(L"PrimaryKey").IsValid();
			bool uniqueConstraint = index.FindChild(L"UniqueKey").IsValid();

			Vector<XmlNode> columns = index.FindChildren(L"Column");
			for (XmlNode& column : columns)
			{
				String colName = column.GetStringAttr(L"name");
			}
		}
	}

	Vector<XmlNode> procedures = root.FindChildren(L"Procedure");
	for (XmlNode& procedure : procedures)
	{
		String name = procedure.GetStringAttr(L"name");
		String body = procedure.FindChild(L"Body").GetStringValue();

		Vector<XmlNode> params = procedure.FindChildren(L"Param");
		for (XmlNode& param : params)
		{
			String paramName = param.GetStringAttr(L"name");
			String paramType = param.GetStringAttr(L"type");
		}
	}

	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\ProjectsV13;Database=ServerDB;Trusted_Connection=Yes;"));

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