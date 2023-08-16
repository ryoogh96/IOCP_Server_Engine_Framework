#pragma once

#include "ServerEnginePCH.hpp"

namespace Engine
{
	class IOCPManager {

	public:
		const void setIOCPHandle(const HANDLE handle) { m_hIOCP = handle; }
		const HANDLE GetIOCPHandle() const { return m_hIOCP; }
		const void setAcceptClientThreadFunc(std::function<void()> func) { m_AcceptClientThreadFunc = func; }
		const std::function<void()> GetAcceptClientThreadFunc() const { return m_AcceptClientThreadFunc; }
		const std::map<SOCKET, Session*> GetSessionMap() const { return m_SessionMap; }

		void AttachSocketToIOCP(const SOCKET socket) const;
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

		void OnServerAccept(IOCPEvent* iocpEvent);
		void OnClientConnect(IOCPEvent* iocpEvent);
		void OnClientDisconnect(IOCPEvent* iocpEvent);

	public:
		IOCPManager();
		~IOCPManager();
	};
}