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
		void attachSocketToIoCompletionPort(const SOCKET& acceptSocket, const Session* session) const;

	private:
		HANDLE m_hIOCP;
		DWORD m_dwThreadCount;
		std::vector<std::thread> m_workerThreads;

	private:
		void StartIoThreads();
		void EndIoThreads();
		void WorkerThreads();

	public:
		IOCPManager();
		~IOCPManager();
	};
}