#pragma once

#include "ServerEnginePCH.hpp"

namespace Engine
{
	class ServerSocketManager
	{
	public:
		ServerSocketManager();
		~ServerSocketManager();

		void createClientSocket();
		void broadcast();

	private:
		const uint8 MAX_CLIENT_SOCKET_POOL = 1;
		SOCKET m_ListenSocket = INVALID_SOCKET;
		Connector* m_Connector = nullptr;
		IOCPManager* m_iocpManager = nullptr;
		map<SOCKET, Session*> m_sessionMap;

		void initialize();
		//const SOCKET createAcceptSocket();
	};
}
