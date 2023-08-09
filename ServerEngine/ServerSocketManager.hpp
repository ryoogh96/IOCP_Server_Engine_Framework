#pragma once

#include "ServerEnginePCH.hpp"

namespace Engine
{
	class ServerSocketManager
	{
	public:
		ServerSocketManager();
		~ServerSocketManager();

		void AcceptClientSocket();
		void Broadcast();

	private:
		SOCKET m_ListenSocket = INVALID_SOCKET;
		Connector* m_Connector = nullptr;
		IOCPManager* m_iocpManager = nullptr;
		map<SOCKET, Session*> m_sessionMap;

		void Initialize();
	};
}
