#include "pch.hpp"
#include "Listener.hpp"
#include "SocketManager.hpp"
#include "IOCPEvent.hpp"
#include "Session.hpp"
#include "Service.hpp"

namespace Engine
{
	Listener::~Listener()
	{
		SocketManager::Close(m_Socket);

		for (AcceptEvent* acceptEvent : m_AcceptEvents)
		{
			xdelete(acceptEvent);
		}
	}

	bool Listener::StartAccept(ServerServiceRef service)
	{
		m_Service = service;
		if (m_Service == nullptr)
			return false;

		m_Socket = SocketManager::CreateSocket();
		if (m_Socket == INVALID_SOCKET)
			return false;

		if (m_Service->GetIOCPManager()->Register(shared_from_this()) == false)
			return false;

		if (SocketManager::SetReuseAddress(m_Socket, true) == false)
			return false;

		if (SocketManager::SetLinger(m_Socket, 0, 0) == false)
			return false;

		if (SocketManager::Bind(m_Socket, m_Service->GetNetAddress()) == false)
			return false;

		if (SocketManager::Listen(m_Socket) == false)
			return false;

		const int32 acceptCount = m_Service->GetMaxSessionCount();
		for (int32 i = 0; i < acceptCount; i++)
		{
			AcceptEvent* acceptEvent = xnew<AcceptEvent>();
			acceptEvent->m_Owner = shared_from_this();
			m_AcceptEvents.push_back(acceptEvent);
			RegisterAccept(acceptEvent);
		}

		return true;
	}

	void Listener::CloseSocket()
	{
		SocketManager::Close(m_Socket);
	}

	HANDLE Listener::GetHandle()
	{
		return reinterpret_cast<HANDLE>(m_Socket);
	}

	void Listener::Dispatch(IOCPEvent* iocpEvent, int32 numOfBytes)
	{
		ASSERT_CRASH(iocpEvent->m_EventType == EVENT_TYPE::ACCEPT);
		AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
		ProcessAccept(acceptEvent);
	}

	void Listener::RegisterAccept(AcceptEvent* acceptEvent)
	{
		SessionRef session = m_Service->CreateSession(); // Register IOCP

		acceptEvent->Init();
		acceptEvent->session = session;

		DWORD bytesReceived = 0;
		if (false == SocketManager::AcceptEx(m_Socket, session->GetSocket(), session->m_RecvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
		{
			const int32 errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				RegisterAccept(acceptEvent);
			}
		}
	}

	void Listener::ProcessAccept(AcceptEvent* acceptEvent)
	{
		SessionRef session = acceptEvent->session;

		if (false == SocketManager::SetUpdateAcceptSocket(session->GetSocket(), m_Socket))
		{
			RegisterAccept(acceptEvent);
			return;
		}

		SOCKADDR_IN sockAddress;
		int32 sizeOfSockAddr = sizeof(sockAddress);
		if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
		{
			RegisterAccept(acceptEvent);
			return;
		}

		session->SetNetAddress(NetAddress(sockAddress));
		session->ProcessConnect();
		RegisterAccept(acceptEvent);
	}
}