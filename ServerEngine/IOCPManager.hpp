#pragma once

#include "ServerEnginePCH.hpp"

namespace Engine
{
	enum class IO_TYPE
	{
		NONE,
		READ,
		WRITE,
		ACCEPT,
		CONNECT,
	};

	class ExtendOverlapped : WSAOVERLAPPED {
	public:
		ExtendOverlapped()
		{
			Internal = 0;
			InternalHigh = 0;
			Offset = 0;
			OffsetHigh = 0;
			hEvent = nullptr;
		}

		IO_TYPE	type = IO_TYPE::NONE;
	};

	class IOCPManager {

	public:
		void AttachSocketToIoCompletionPort(const SOCKET& acceptSocket, const Session* session) const;

	private:
		const uint8 MAX_CLIENT_SOCKET_POOL = 1;
		HANDLE m_hIOCP;
		DWORD m_dwThreadCount;
		std::vector<std::thread> m_workerThreads;

	private:
		void StartAcceptClientThreads();
		void StartWorkerThreads();
		void EndThreads();
		void WorkerThreads();

	public:
		IOCPManager();
		~IOCPManager();
	};
}