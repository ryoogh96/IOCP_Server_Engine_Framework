#include "pch.hpp"

#include "IOCPManager.hpp"

namespace Engine
{
	IOCPManager::IOCPManager()
	{
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		ASSERT_CRASH(m_hIOCP != INVALID_HANDLE_VALUE);
		m_dwThreadCount = std::thread::hardware_concurrency();
		StartWorkerThreads();
	}

	IOCPManager::~IOCPManager()
	{
		CloseHandle(m_hIOCP);
		EndThreads();
	}

	void IOCPManager::AttachListenSocketToIOCP(const SOCKET listenSocket) const
	{
		if (::CreateIoCompletionPort(reinterpret_cast<const HANDLE>(listenSocket), m_hIOCP, 0, 0) == INVALID_HANDLE_VALUE)
		{
			std::cout << "IOCPManager::AttachListenSocketToIOCP()" << std::endl;
			std::cout << "CreateIoCompletionPort WSAGetLastError: " << ::WSAGetLastError() << std::endl;
		}
	}

	void IOCPManager::AttachAcceptSocketToIOCP(const SOCKET acceptSocket) const
	{
		if (::CreateIoCompletionPort(reinterpret_cast<const HANDLE>(acceptSocket), m_hIOCP, 0, 0) == INVALID_HANDLE_VALUE)
		{
			std::cout << "Listener::AttachAcceptClientSocketToIOCP()" << std::endl;
			std::cout << "CreateIoCompletionPort WSAGetLastError: " << ::WSAGetLastError() << std::endl;
		}
	}


	void IOCPManager::WorkerThreads()
	{
		while (true)
		{
			DWORD bytesTransferred = 0;
			Session* session = nullptr;
			ExtendOverlapped* extendOverlapped = nullptr;
			const BOOL status = ::GetQueuedCompletionStatus(m_hIOCP, &bytesTransferred, (ULONG_PTR*)&session, (LPOVERLAPPED*)&extendOverlapped, INFINITE);

			if (status == FALSE || bytesTransferred == 0)
			{
				if (extendOverlapped->m_type == IO_TYPE::ACCEPT)
				{
					std::cout << "extendOverlapped->type == IO_TYPE::ACCEPT" << std::endl;

					if (m_RemainAcceptSocketPool <= 0)
					{
						std::cout << "socket pool already full" << std::endl;
						continue;
					}

					Session* session = new Session();
					session->SetSocket(extendOverlapped->m_acceptSocket);
					AttachAcceptSocketToIOCP(session->GetSocket());

					m_SessionMap.insert(std::make_pair(session->GetSocket(), session));

					std::cout << "session->socket: " << session->GetSocket() << " has been created" << std::endl;

					WSABUF recvWSABuf;
					recvWSABuf.buf = reinterpret_cast<char*>(session->GetRecvBuffer());
					recvWSABuf.len = MAX_BUF_SIZE;
					DWORD recvLen = 0;
					DWORD flags = 0;
					extendOverlapped->m_type = IO_TYPE::READ;
					if (SOCKET_ERROR == ::WSARecv(session->GetSocket(), &recvWSABuf, 1, &recvLen, &flags, (LPWSAOVERLAPPED)extendOverlapped, nullptr))
					{
						const int lastError = ::WSAGetLastError();
						if (lastError != WSA_IO_PENDING && lastError != WSAENOTCONN)
						{
							std::cout << "Listener::CreateAcceptSocket()" << std::endl;
							std::cout << "::WSARecv WSAGetLastError: " << lastError << std::endl;
							ASSERT_CRASH(lastError == WSA_IO_PENDING || lastError == WSAENOTCONN);
							continue;
						}
					}

					m_RemainAcceptSocketPool--;
					continue;
				}

				if (session == nullptr && extendOverlapped->m_acceptSocket == INVALID_SOCKET)
				{
					continue;
				}

				session = m_SessionMap.find(extendOverlapped->m_acceptSocket)->second;
				m_SessionMap.erase(extendOverlapped->m_acceptSocket);

				std::cout << "session->socket: " << session->GetSocket() << " has been disconnected" << std::endl;
				extendOverlapped->m_type = IO_TYPE::ACCEPT;
				::closesocket(session->GetSocket());
				session->SetSocket(INVALID_SOCKET);
				delete session;
				// FIXME: why abort() call error occur?
				//m_AcceptClientThreads.erase(m_AcceptClientThreads.begin());
				//m_AcceptClientThreads.pop_back();
				m_AcceptClientThreads.emplace_back([this]() { this->GetAcceptClientThreadFunc()(); });
				m_RemainAcceptSocketPool++;
				
				continue;
			}

			DWORD recvLen = 0;
			DWORD flags = 0;

			if (extendOverlapped->m_type == IO_TYPE::READ)
			{
				std::cout << "extendOverlapped->type == IO_TYPE::READ" << std::endl;
				std::cout << "session" << session << std::endl;
				//std::cout << "session->GetSocket()" << session->GetSocket() << std::endl;
				//std::cout << "session->recvBuffer:" << session->GetSendBuffer() << std::endl;
				// echo
				//WSABUF sendWSABuf;
				//sendWSABuf.buf = reinterpret_cast<char*>(session->GetSendBuffer());
				//sendWSABuf.len = bytesTransferred;
				//extendOverlapped->type = IO_TYPE::WRITE;
				//::WSASend(session->getSocket(), &sendWSABuf, 1, &recvLen, flags, (LPWSAOVERLAPPED)&extendOverlapped, nullptr);

				WSABUF recvWSABuf;
				recvWSABuf.buf = reinterpret_cast<char*>(session->GetSendBuffer());
				recvWSABuf.len = MAX_BUF_SIZE;
				extendOverlapped->m_type = IO_TYPE::READ;
				if (::WSARecv(session->GetSocket(), &recvWSABuf, 1, &recvLen, &flags, (LPWSAOVERLAPPED)extendOverlapped, nullptr) == SOCKET_ERROR)
				{
					std::cout << "WSARecv WSAGetLastError: " << ::WSAGetLastError() << std::endl;
				}
			}
			else if (extendOverlapped->m_type == IO_TYPE::WRITE)
			{
				std::cout << "extendOverlapped->type == IO_TYPE::WRITE" << std::endl;
				std::cout << "session->sendBuffer:" << session->GetSendBuffer() << std::endl;

				// delete with static size array occur heap validate error
				// delete session->getSendBuffer();
			}
			else
			{
				printf("unknown extendOverlapped->type: %d\n", extendOverlapped->m_type);
			}
		}
	}

	void IOCPManager::StartAcceptClientThreads()
	{
		for (uint8 i = 0; i < MAX_ACCPET_SOCKET_POOL; ++i)
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

		std:: cout << "available threads num: " << m_dwThreadCount - MAX_ACCPET_SOCKET_POOL << std::endl;
	}

	void IOCPManager::EndThreads()
	{
		for (auto& threads : m_WorkerThreads) {
			threads.join();
		}
	}
}