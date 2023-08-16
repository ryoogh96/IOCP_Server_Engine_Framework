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
		StartWorkerThreads();
	}

	IOCPManager::~IOCPManager()
	{
		CloseHandle(m_hIOCP);
		EndThreads();
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
		std::cout << "extendOverlapped->type == IO_TYPE::SERVER_ACCEPT" << std::endl;

		if (m_RemainAcceptSocketPool <= 0)
		{
			std::cout << "socket pool already full" << std::endl;
			return;
		}

		Session* session = new Session();
		session->SetSocket(iocpEvent->GetSocket());
		AttachSocketToIOCP(session->GetSocket());

		m_SessionMap.insert(std::make_pair(session->GetSocket(), session));

		std::cout << "session->socket: " << session->GetSocket() << " has been created" << std::endl;

		WSABUF recvWSABuf;
		recvWSABuf.buf = reinterpret_cast<char*>(session->GetRecvBuffer());
		recvWSABuf.len = MAX_BUF_SIZE;
		DWORD recvLen = 0;
		DWORD flags = 0;
		iocpEvent->SetIOType(IO_TYPE::SERVER_RECV);
		if (SOCKET_ERROR == ::WSARecv(session->GetSocket(), &recvWSABuf, 1, &recvLen, &flags, iocpEvent, nullptr))
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
		Session* session = m_SessionMap.find(iocpEvent->GetSocket())->second;
		m_SessionMap.erase(iocpEvent->GetSocket());

		std::cout << "session->socket: " << session->GetSocket() << " has been disconnected" << std::endl;
		
		::closesocket(session->GetSocket());
		session->SetSocket(INVALID_SOCKET);

		delete session;
		
		// FIXME: why abort() call error occur?
		//m_AcceptClientThreads.erase(m_AcceptClientThreads.begin());
		//m_AcceptClientThreads.pop_back();
		m_AcceptClientThreads.emplace_back([this]() { this->GetAcceptClientThreadFunc()(); });
		m_RemainAcceptSocketPool++;
	}


	void IOCPManager::WorkerThreads()
	{
		while (true)
		{
			DWORD bytesTransferred = 0;
			Session* session = nullptr;
			IOCPEvent* iocpEvent = nullptr;
			const BOOL status = ::GetQueuedCompletionStatus(m_hIOCP, &bytesTransferred, reinterpret_cast<ULONG_PTR*>(&session), reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), INFINITE);

			DWORD recvLen = 0;
			DWORD flags = 0;
			if (status == FALSE || bytesTransferred == 0)
			{
				if (iocpEvent->GetIOType() == IO_TYPE::SERVER_ACCEPT)
				{
					OnServerAccept(iocpEvent);
				}
				else if (iocpEvent->GetIOType() == IO_TYPE::CLIENT_CONNECT)
				{
					OnClientConnect(iocpEvent);
				}
				else if (iocpEvent->GetIOType() == IO_TYPE::CLIENT_DISCONNECT)
				{
					OnClientDisconnect(iocpEvent);
				}
				else
				{
					OnClientDisconnect(iocpEvent);
				}
			}
			else if (iocpEvent->GetIOType() == IO_TYPE::SERVER_SEND)
			{
				std::cout << "extendOverlapped->type == IO_TYPE::SERVER_SEND" << std::endl;
				std::cout << "session->sendBuffer:" << session->GetSendBuffer() << std::endl;

				// delete with static size array occur heap validate error
				// delete session->getSendBuffer();
			}
			else if (iocpEvent->GetIOType() == IO_TYPE::SERVER_RECV)
			{
				Session* _session = m_SessionMap.find(iocpEvent->GetSocket())->second;

				std::cout << "extendOverlapped->type == IO_TYPE::SERVER_RECV" << std::endl;
				std::cout << "session" << _session << std::endl;
				std::cout << "session->GetSocket(): " << _session->GetSocket() << std::endl;
				std::cout << "session->recvBuffer:" << _session->GetSendBuffer() << std::endl;
				// echo
				//WSABUF sendWSABuf;
				//sendWSABuf.buf = reinterpret_cast<char*>(session->GetSendBuffer());
				//sendWSABuf.len = bytesTransferred;
				//iocpEvent->SetIOType(IO_TYPE::SERVER_SEND);
				//::WSASend(session->GetSocket(), &sendWSABuf, 1, &recvLen, flags, iocpEvent, nullptr);

				WSABUF recvWSABuf;
				recvWSABuf.buf = reinterpret_cast<char*>(_session->GetSendBuffer());
				recvWSABuf.len = MAX_BUF_SIZE;
				iocpEvent->SetIOType(IO_TYPE::SERVER_RECV);
				if (::WSARecv(_session->GetSocket(), &recvWSABuf, 1, &recvLen, &flags, iocpEvent, nullptr) == SOCKET_ERROR)
				{
					std::cout << "WSARecv WSAGetLastError: " << ::WSAGetLastError() << std::endl;
				}
			}
			else if (iocpEvent->GetIOType() == IO_TYPE::CLIENT_SEND)
			{
				std::cout << "iocpEvent->GetIOType() == IO_TYPE::CLIENT_SEND" << std::endl;
			}
			else if (iocpEvent->GetIOType() == IO_TYPE::CLIENT_RECV)
			{
				std::cout << "iocpEvent->GetIOType() == IO_TYPE::CLIENT_RECV" << std::endl;
			}
			else
			{
				printf("unknown extendOverlapped->type: %d\n", iocpEvent->GetIOType());
			}
		}
	}

	void IOCPManager::StartAcceptClientThreads()
	{
		for (uint16 i = 0; i < MAX_ACCPET_SOCKET_POOL; ++i)
		{
			m_AcceptClientThreads.emplace_back([this]() { this->GetAcceptClientThreadFunc()(); });
		}
	}

	void IOCPManager::StartWorkerThreads()
	{
		for (uint16 i = 0; i < m_dwThreadCount - MAX_ACCPET_SOCKET_POOL; ++i)
		{
			m_WorkerThreads.emplace_back([this]() { this->WorkerThreads(); });
		}
	}

	void IOCPManager::EndThreads()
	{
		for (auto& threads : m_WorkerThreads) {
			threads.join();
		}
	}
}