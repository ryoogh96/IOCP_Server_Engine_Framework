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

		IO_TYPE	m_type = IO_TYPE::NONE;
		char m_acceptBuf[512] = { 0, };
		SOCKET m_acceptSocket = INVALID_SOCKET;
	};

	class IOCPManager {

	public:
		const void setIOCPHandle(const HANDLE handle) { m_hIOCP = handle; }
		const HANDLE GetIOCPHandle() const { return m_hIOCP; }
		const void setAcceptClientThreadFunc(std::function<void()> func) { m_AcceptClientThreadFunc = func; }
		const std::function<void()> GetAcceptClientThreadFunc() const { return m_AcceptClientThreadFunc; }
		const std::map<SOCKET, Session*> GetSessionMap() const { return m_SessionMap; }

		void AttachListenSocketToIOCP(const SOCKET listenSocket) const;
		void AttachAcceptSocketToIOCP(const SOCKET acceptSocket) const;
		void StartAcceptClientThreads();

	private:
		uint8 m_RemainAcceptSocketPool = 1;
		const uint8 MAX_ACCPET_SOCKET_POOL = 1;
		HANDLE m_hIOCP;
		DWORD m_dwThreadCount;
		std::vector<std::thread> m_AcceptClientThreads;
		std::vector<std::thread> m_WorkerThreads;
		std::function<void()> m_AcceptClientThreadFunc;
		std::map<SOCKET, Session*> m_SessionMap;

	private:
		void StartWorkerThreads();
		void EndThreads();
		void WorkerThreads();

	public:
		IOCPManager();
		~IOCPManager();
	};
}