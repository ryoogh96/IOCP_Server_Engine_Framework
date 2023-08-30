#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.hpp"
#include "Thread/ThreadManager.hpp"
#include "Network/Service.hpp"
#include "Network/Session.hpp"
#include "Utility/BufferReader.hpp"
#include "ClientPacketHandler.hpp"

using namespace Engine;

char sendData[] = "Hello World";

class ServerSession : public Engine::PacketSession
{
public:
	~ServerSession()
	{
		std::cout << "~ServerSession" << std::endl;
	}

	virtual void OnConnected() override
	{
		std::cout << "Connected To Server" << std::endl;
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		ClientPacketHandler::HandlePacket(buffer, len);
	}

	virtual void OnSend(Engine::int32 len) override
	{
		std::cout << "OnSend Len = " << len << std::endl;
	}

	virtual void OnDisconnected() override
	{
		std::cout << "Disconnected" << std::endl;
	}
};

int main()
{
	Engine::MiniDump minidump;
	minidump.BeginDump();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	Engine::ClientServiceRef service = Engine::MakeShared<Engine::ClientService>(
		Engine::NetAddress(L"127.0.0.1", 7777),
		Engine::MakeShared<Engine::IOCPManager>(),
		Engine::MakeShared<ServerSession>,
		1000);

	ASSERT_CRASH(service->Start());

	for (Engine::int32 i = 0; i < 2; i++)
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
