#pragma once

#include "IOCPManager.hpp"
#include "IOCPEvent.hpp"
#include "NetAddress.hpp"
#include "RecvBuffer.hpp"

namespace Engine
{
	class Service;

	class Session : public IOCPObject
	{
		friend class Listener;
		friend class IOCPManager;
		friend class Service;

		enum
		{
			BUFFER_SIZE = 0x10000, // 64KB
		};

	public:
		Session();
		virtual ~Session();

		void						Send(SendBufferRef sendBuffer);
		bool						Connect();
		void						Disconnect(const WCHAR* cause);

		std::shared_ptr<Service>	GetService() { return m_Service.lock(); }
		void						SetService(std::shared_ptr<Service> service) { m_Service = service; }

	private:
		void						SetNetAddress(NetAddress address) { m_NetAddress = address; }
		NetAddress					GetAddress() { return m_NetAddress; }
		SOCKET						GetSocket() { return m_Socket; }
		bool						IsConnected() { return m_Connected; }
		SessionRef					GetSessionRef() { return std::static_pointer_cast<Session>(shared_from_this()); }

	private:
		virtual HANDLE				GetHandle() override;
		virtual	void				Dispatch(class IOCPEvent* iocpEvent, int32 numOfByte = 0) override;

	private:
		bool						RegisterConnect();
		bool						RegisterDisconnect();
		void						RegisterRecv();
		void						RegisterSend();

		void						ProcessConnect();
		void						ProcessDisconnect();
		void						ProcessRecv(int32 numOfBytes);
		void						ProcessSend(int32 numOfBytes);

		void						HandleError(int32 errorCode);

	protected:
		virtual void				OnConnected() { }
		virtual int32				OnRecv(BYTE* buffer, int32 len) { return len; }
		virtual void				OnSend(int32 len) { }
		virtual void				OnDisconnected() { }

	private:
		std::weak_ptr<Service>		m_Service;
		SOCKET						m_Socket = INVALID_SOCKET;
		NetAddress					m_NetAddress = {};
		Atomic<bool>				m_Connected = false;

	private:
		USE_LOCK;

		RecvBuffer				m_RecvBuffer;

		Queue<SendBufferRef>	m_SendQueue;
		Atomic<bool>			m_SendRegistered = false;

	private:
		ConnectEvent	m_ConnectEvent;
		DisconnectEvent	m_DisconnectEvent;
		RecvEvent		m_RecvEvent;
		SendEvent		m_SendEvent;
	};

	struct PacketHeader
	{
		uint16 size;
		uint16 id; // protocol ID (ex. 1=login, 2=requestMove)
	};

	class PacketSession : public Session
	{
	public:
		PacketSession();
		virtual ~PacketSession();

		PacketSessionRef	GetPacketSessionRef() { return std::static_pointer_cast<PacketSession>(shared_from_this()); }

	protected:
		virtual int32		OnRecv(BYTE* buffer, int32 len) sealed;
		virtual int32		OnRecvPacket(BYTE* buffer, int32 len) abstract;
	};
}

