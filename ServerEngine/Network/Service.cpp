#include "pch.hpp"
#include "Service.hpp"
#include "Session.hpp"
#include "Listener.hpp"

namespace Engine
{
	Service::Service(ServiceType type, NetAddress address, IOCPManagerRef iocpManager, SessionFactory factory, int32 maxSessionCount)
		: m_Type(type), m_NetAddress(address), m_IOCPManager(iocpManager), m_SessionFactory(factory), m_MaxSessionCount(maxSessionCount)
	{

	}

	Service::~Service()
	{
	}

	void Service::CloseService()
	{

	}

	SessionRef Service::CreateSession()
	{
		SessionRef session = m_SessionFactory();
		session->SetService(shared_from_this());

		if (m_IOCPManager->Register(session) == false)
			return nullptr;

		return session;
	}

	void Service::AddSession(SessionRef session)
	{
		WRITE_LOCK;
		m_SessionCount++;
		m_Sessions.insert(session);
	}

	void Service::ReleaseSession(SessionRef session)
	{
		WRITE_LOCK;
		ASSERT_CRASH(m_Sessions.erase(session) != 0);
		m_SessionCount--;
	}

	/*-----------------
		ClientService
	------------------*/

	ClientService::ClientService(NetAddress targetAddress, IOCPManagerRef iocpManager, SessionFactory factory, int32 maxSessionCount)
		: Service(ServiceType::Client, targetAddress, iocpManager, factory, maxSessionCount)
	{
	}

	bool ClientService::Start()
	{
		if (CanStart() == false)
			return false;

		const int32 sessionCount = GetMaxSessionCount();
		for (int32 i = 0; i < sessionCount; i++)
		{
			SessionRef session = CreateSession();
			if (session->Connect() == false)
				return false;
		}

		return true;
	}

	ServerService::ServerService(NetAddress address, IOCPManagerRef iocpManager, SessionFactory factory, int32 maxSessionCount)
		: Service(ServiceType::Server, address, iocpManager, factory, maxSessionCount)
	{
	}

	bool ServerService::Start()
	{
		if (CanStart() == false)
			return false;

		m_Listener = MakeShared<Listener>();
		if (m_Listener == nullptr)
			return false;

		ServerServiceRef service = std::static_pointer_cast<ServerService>(shared_from_this());
		if (m_Listener->StartAccept(service) == false)
			return false;

		return true;
	}

	void ServerService::CloseService()
	{
		Service::CloseService();
	}

}