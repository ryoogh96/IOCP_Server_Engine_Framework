#include "pch.hpp"

#include "IOCPManager.hpp"

namespace Engine
{
	IOCPManager::IOCPManager()
	{
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
		ASSERT_CRASH(m_hIOCP != INVALID_HANDLE_VALUE);
		m_dwThreadCount = std::thread::hardware_concurrency();
		StartWorkerThreads();
	}

	IOCPManager::~IOCPManager()
	{
		CloseHandle(m_hIOCP);
		EndThreads();
	}

	void IOCPManager::AttachSocketToIoCompletionPort(const SOCKET& acceptSocket, const Session* session) const
	{
		if (::CreateIoCompletionPort(reinterpret_cast<const HANDLE>(acceptSocket), m_hIOCP, (ULONG_PTR)session, 0) == INVALID_HANDLE_VALUE)
		{
			std::cout << "IOCPManager::attachSocketToIoCompletionPort()" << std::endl;
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
				if (session == nullptr) return;

				std::cout << "::GetQueuedCompletionStatus()" << std::endl;
				std::cout << "session->socket: " << session->GetSocket() << " has been disconnected" << std::endl;

				// TODO: delete session, and if it is server then remove it from m_sessionMap;
				delete session;

				return;
			}

			DWORD recvLen = 0;
			DWORD flags = 0;

			if (extendOverlapped->type == IO_TYPE::READ)
			{
				std::cout << "extendOverlapped->type == IO_TYPE::READ" << std::endl;
				std::cout << "session->recvBuffer:" << session->GetSendBuffer() << std::endl;
				// echo
				//WSABUF sendWSABuf;
				//sendWSABuf.buf = reinterpret_cast<char*>(session->GetSendBuffer());
				//sendWSABuf.len = bytesTransferred;
				//extendOverlapped->type = IO_TYPE::WRITE;
				//::WSASend(session->getSocket(), &sendWSABuf, 1, &recvLen, flags, (LPWSAOVERLAPPED)&extendOverlapped, nullptr);

				WSABUF recvWSABuf;
				recvWSABuf.buf = reinterpret_cast<char*>(session->GetSendBuffer());
				recvWSABuf.len = MAX_BUF_SIZE;
				extendOverlapped->type = IO_TYPE::READ;
				if (::WSARecv(session->GetSocket(), &recvWSABuf, 1, &recvLen, &flags, (LPWSAOVERLAPPED)extendOverlapped, nullptr) == SOCKET_ERROR)
				{
					std::cout << "WSARecv WSAGetLastError: " << ::WSAGetLastError() << std::endl;
				}
			}
			else if (extendOverlapped->type == IO_TYPE::WRITE)
			{
				std::cout << "extendOverlapped->type == IO_TYPE::WRITE" << std::endl;
				std::cout << "session->sendBuffer:" << session->GetSendBuffer() << std::endl;

				// delete with static size array occur heap validate error
				// delete session->getSendBuffer();
			}
			else
			{
				printf("unknown extendOverlapped->type: %d\n", extendOverlapped->type);
			}
		}
	}

	void IOCPManager::StartAcceptClientThreads()
	{
		for (uint16 i = 0; i < MAX_CLIENT_SOCKET_POOL; ++i)
		{
			m_workerThreads.emplace_back([this]() { this->WorkerThreads(); });
		}
	}

	void IOCPManager::StartWorkerThreads()
	{
		for (uint16 i = 0; i < m_dwThreadCount - MAX_CLIENT_SOCKET_POOL; ++i)
		{
			m_workerThreads.emplace_back([this]() { this->WorkerThreads(); });
		}
	}

	void IOCPManager::EndThreads()
	{
		for (auto& threads : m_workerThreads) {
			threads.join();
		}
	}
}