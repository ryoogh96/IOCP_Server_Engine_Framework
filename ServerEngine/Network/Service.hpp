#pragma once

#include "NetAddress.hpp"
#include "IOCPManager.hpp"
#include "Listener.hpp"
#include <functional>

namespace Engine
{
	enum class ServiceType : uint8
	{
		Server,
		Client
	};

	using SessionFactory = std::function<SessionRef(void)>;

	class Service : public std::enable_shared_from_this<Service>
	{
	public:
		Service(ServiceType type, NetAddress address, IOCPManagerRef iocpManager, SessionFactory factory, int32 maxSessionCount = 1);
		virtual ~Service();

		virtual bool	Start() abstract;
		bool			CanStart() { return m_SessionFactory != nullptr; }

		virtual void	CloseService();
		void			SetSessionFactory(SessionFactory sessionFactory) { m_SessionFactory = sessionFactory; }

		void			Broadcast(SendBufferRef sendBuffer);
		SessionRef		CreateSession();
		void			AddSession(SessionRef session);
		void			ReleaseSession(SessionRef session);
		int32			GetCurrentSessionCount() { return m_SessionCount; }
		int32			GetMaxSessionCount() { return m_MaxSessionCount; }

	public:
		ServiceType		GetServiceType() { return m_Type; }
		NetAddress		GetNetAddress() { return m_NetAddress; }
		IOCPManagerRef& GetIOCPManager() { return m_IOCPManager; }

	protected:
		USE_LOCK;
		ServiceType		m_Type;
		NetAddress		m_NetAddress = {};
		IOCPManagerRef	m_IOCPManager;

		Set<SessionRef> m_Sessions;
		int32			m_SessionCount = 0;
		int32			m_MaxSessionCount = 0;
		SessionFactory	m_SessionFactory;
	};

	class ClientService : public Service
	{
	public:
		ClientService(NetAddress targetAddress, IOCPManagerRef iocpManager, SessionFactory factory, int32 maxSessionCount = 1);
		virtual ~ClientService() {}

		virtual bool Start() override;
	};

	class ServerService : public Service
	{
	public:
		ServerService(NetAddress targetAddress, IOCPManagerRef iocpManager, SessionFactory factory, int32 maxSessionCount = 1);
		virtual ~ServerService() {}

		virtual bool	Start() override;
		virtual void	CloseService() override;

	private:
		ListenerRef		m_Listener = nullptr;
	};
}