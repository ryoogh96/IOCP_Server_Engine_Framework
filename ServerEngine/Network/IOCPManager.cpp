#include "pch.hpp"

#include "IOCPManager.hpp"

namespace Engine
{
	IOCPManager::IOCPManager()
	{
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		ASSERT_CRASH(m_hIOCP != INVALID_HANDLE_VALUE);
		m_dwThreadCount = std::thread::hardware_concurrency();
		std::cout << "available total threads num: " << m_dwThreadCount << std::endl;
	}

	IOCPManager::~IOCPManager()
	{
		CloseHandle(m_hIOCP);
		GThreadManager->Join();
	}

	void IOCPManager::AttachSocketToIOCP(const SOCKET socket) const
	{
		if (::CreateIoCompletionPort(reinterpret_cast<const HANDLE>(socket), m_hIOCP, 0, 0) == INVALID_HANDLE_VALUE)
		{
			std::cout << "IOCPManager::AttachSocketToIOCP()" << std::endl;
			std::cout << "CreateIoCompletionPort WSAGetLastError: " << ::WSAGetLastError() << std::endl;
		}
	}

	void IOCPManager::OnServerAccept(IOCPEvent* iocpEvent)
	{
		std::cout << "extendOverlapped->type == EVENT_TYPE::ACCEPT" << std::endl;

		if (m_RemainAcceptSocketPool <= 0)
		{
			std::cout << "socket pool already full" << std::endl;
			return;
		}

		WRITE_LOCK;

		SessionRef session = MakeShared<Session>();
		session->SetSocket(iocpEvent->GetSocket());
		const SOCKET socket = session->GetSocket();
		AttachSocketToIOCP(socket);

		m_SessionMap.insert(std::make_pair(socket, session));

		std::cout << "session->socket: " << socket << " has been created" << std::endl;

		RecvBuffer recvBuffer = session->GetRecvBuffer();

		WSABUF recvWSABuf;
		recvWSABuf.buf = reinterpret_cast<char*>(recvBuffer.WritePos());
		recvWSABuf.len = recvBuffer.FreeSize();
		DWORD recvLen = 0;
		DWORD flags = 0;
		iocpEvent->SetIOType(EVENT_TYPE::RECV);
		if (SOCKET_ERROR == ::WSARecv(socket, &recvWSABuf, 1, OUT &recvLen, OUT &flags, iocpEvent, nullptr))
		{
			const int lastError = ::WSAGetLastError();
			if (lastError != WSA_IO_PENDING && lastError != WSAENOTCONN)
			{
				std::cout << "IOCPManager::OnServerAccept()" << std::endl;
				std::cout << "::WSARecv WSAGetLastError: " << lastError << std::endl;
				ASSERT_CRASH(lastError == WSA_IO_PENDING || lastError == WSAENOTCONN);
			}
		}

		m_RemainAcceptSocketPool--;
	}

	void IOCPManager::OnClientConnect(IOCPEvent* iocpEvent)
	{
		std::cout << "session->socket: " << iocpEvent->GetSocket() << " has been connected" << std::endl;

		if (WAIT_TIMEOUT == ::WaitForSingleObject(reinterpret_cast<HANDLE>(iocpEvent->GetSocket()), 3000))
		{
			const int lastError = ::WSAGetLastError();
			std::cout << "IOCPManager::OnClientConnect()" << std::endl;
			std::cout << "::WaitForSingleObject WSAGetLastError: " << lastError << std::endl;
			ASSERT_CRASH(lastError != WAIT_TIMEOUT);
		}
	}

	void IOCPManager::OnClientDisconnect(IOCPEvent* iocpEvent)
	{
		const auto sessionMap = m_SessionMap.find(iocpEvent->GetSocket());

		if (sessionMap == m_SessionMap.end())
		{
			return;
		}

		WRITE_LOCK;

		SessionRef session = sessionMap->second;
		m_SessionMap.erase(iocpEvent->GetSocket());

		std::cout << "session->socket: " << session->GetSocket() << " has been disconnected" << std::endl;
		
		::closesocket(session->GetSocket());
		session->SetSocket(INVALID_SOCKET);
		
		m_RemainAcceptSocketPool++;
	}

	void IOCPManager::OnServerRecv(IOCPEvent* iocpEvent)
	{
		const auto sessionMap = m_SessionMap.find(iocpEvent->GetSocket());

		if (sessionMap == m_SessionMap.end()) return;

		const SessionRef _session = sessionMap->second;

		std::cout << "session" << _session << std::endl;
		std::cout << "session->GetSocket(): " << _session->GetSocket() << std::endl;

		DWORD recvLen = 0;
		DWORD flags = 0;

		// echo
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
		::memcpy(sendBuffer->Buffer(), "echo back", sizeof("echo back"));
		sendBuffer->Close(sizeof("echo back"));

		std::cout << "extendOverlapped->type == EVENT_TYPE::RECV" << std::endl;

		RecvBuffer recvBuffer = _session->GetRecvBuffer();
		std::cout << "session->recvBuffer:" << recvBuffer.DataSize() << std::endl;

		if (recvBuffer.onWrite(sizeof(iocpEvent->GetBuffer())) == false)
		{
			// Disconnect("OnWrite Overflow");
			std::cout << "OnWrite Overflow" << std::endl;
			return;
		}

		recvBuffer.Clean();

		WSABUF recvWSABuf;
		recvWSABuf.buf = reinterpret_cast<char*>(recvBuffer.WritePos());
		recvWSABuf.len = recvBuffer.FreeSize();
		iocpEvent->SetIOType(EVENT_TYPE::RECV);
		if (::WSARecv(_session->GetSocket(), &recvWSABuf, 1, &recvLen, &flags, iocpEvent, nullptr) == SOCKET_ERROR)
		{
			std::cout << "WSARecv WSAGetLastError: " << ::WSAGetLastError() << std::endl;
		}


		std::cout << "extendOverlapped->type == EVENT_TYPE::RECV" << std::endl;
	}


	void IOCPManager::WorkerThreads()
	{
		while (true)
		{
			DWORD bytesTransferred = 0;
			Session* session = nullptr;
			IOCPEvent* iocpEvent = nullptr;
			const BOOL status = ::GetQueuedCompletionStatus(m_hIOCP, OUT &bytesTransferred, OUT reinterpret_cast<ULONG_PTR*>(&session), OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), INFINITE);

			if (status == FALSE || bytesTransferred == 0)
			{
				if (iocpEvent->GetEventType() == EVENT_TYPE::ACCEPT)
				{
					OnServerAccept(iocpEvent);
				}
				else if (iocpEvent->GetEventType() == EVENT_TYPE::CONNECT)
				{
					OnClientConnect(iocpEvent);
				}
				else if (iocpEvent->GetEventType() == EVENT_TYPE::DISCONNECT)
				{
					OnClientDisconnect(iocpEvent);
				}
				else
				{
					OnClientDisconnect(iocpEvent);
				}
			}
			else if (iocpEvent->GetEventType() == EVENT_TYPE::SEND)
			{
				std::cout << "extendOverlapped->type == EVENT_TYPE::SEND" << std::endl;
				//std::cout << "session->recvBuffer:" << session->GetRecvBuffer().DataSize() << std::endl;
			}
			else if (iocpEvent->GetEventType() == EVENT_TYPE::RECV)
			{
				OnServerRecv(iocpEvent);
			}
			else if (iocpEvent->GetEventType() == EVENT_TYPE::SEND)
			{
				std::cout << "iocpEvent->GetEventType() == EVENT_TYPE::SEND" << std::endl;
			}
			else if (iocpEvent->GetEventType() == EVENT_TYPE::RECV)
			{
				std::cout << "iocpEvent->GetEventType() == EVENT_TYPE::RECV" << std::endl;
			}
			else
			{
				printf("unknown extendOverlapped->type: %d\n", iocpEvent->GetEventType());
			}
		}
	}

	void IOCPManager::StartAcceptClientThreads()
	{
		for (uint16 i = 0; i < MAX_ACCPET_SOCKET_POOL; ++i)
		{
			GThreadManager->Launch(GetAcceptClientThreadFunc());
		}
	}

	void IOCPManager::StartWorkerThreads()
	{
		for (uint16 i = 0; i < m_dwThreadCount - MAX_ACCPET_SOCKET_POOL; ++i)
		{
			GThreadManager->Launch([this]() { this->WorkerThreads(); });
		}
	}
}