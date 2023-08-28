#include "pch.hpp"

#include "ServerSocketManager.hpp"

namespace Engine
{
	ServerSocketManager::ServerSocketManager()
	{
		Initialize();
	}

	ServerSocketManager::~ServerSocketManager()
	{
		WRITE_LOCK;
		for (const auto sessionMap : m_iocpManager->GetSessionMap())
		{
			const SOCKET socket = sessionMap.second->GetSocket();
			if (socket != INVALID_SOCKET)
				::closesocket(socket);
			sessionMap.second->SetSocket(INVALID_SOCKET);
		}

		delete m_Listener;
		delete m_Connector;
		delete m_iocpManager;

		::WSACleanup();
	}

	void ServerSocketManager::Initialize()
	{
		WSADATA wsaData;
		const int wsaStartup = ::WSAStartup(MAKEWORD(2, 2), OUT & wsaData);
		if (wsaStartup != 0)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::WSAStartup Error: " << wsaStartup << std::endl;
			ASSERT_CRASH(wsaStartup == 0);
		}

		m_Listener = new Listener();

		m_Listener->setListenSocket(::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED));
		if (m_Listener->getListenSocket() == INVALID_SOCKET)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::WSASocketW WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(m_Listener->getListenSocket() != INVALID_SOCKET);
		}

		constexpr int opt = 1;
		const int sockopt = ::setsockopt(m_Listener->getListenSocket(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(int));
		if (sockopt == SOCKET_ERROR)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::setsockopt WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(sockopt != SOCKET_ERROR);
		}

		IN_ADDR address;
		::InetPtonW(AF_INET, SERVER_IP, &address);
		SOCKADDR_IN sockAddr;
		ZeroMemory(&sockAddr, sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = ::htons(DEFAULT_PORT);
		sockAddr.sin_addr = address;

		const int bind = ::bind(m_Listener->getListenSocket(), reinterpret_cast<const SOCKADDR*>(&sockAddr), sizeof(sockAddr)) != SOCKET_ERROR;
		if (bind == SOCKET_ERROR)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::bind WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(bind != SOCKET_ERROR);
		}
		const int listen = ::listen(m_Listener->getListenSocket(), SOMAXCONN);
		if (listen == SOCKET_ERROR)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::listen WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(listen != SOCKET_ERROR);
		}

		m_iocpManager = new IOCPManager();
		m_iocpManager->AttachSocketToIOCP(m_Listener->getListenSocket());
		m_iocpManager->setAcceptClientThreadFunc(std::bind(&Listener::CreateAcceptSocket, m_Listener, m_iocpManager->GetIOCPHandle(), m_iocpManager->GetSessionMap()));
		m_iocpManager->StartAcceptClientThreads();
	}

	void ServerSocketManager::Broadcast()
	{
		char msg[] = "message broadcasting...";
		for (const auto sessionMap : m_iocpManager->GetSessionMap())
		{
			SessionRef session = sessionMap.second;
			session->Send(msg);

			std::cout << "session->socket: " << session->GetSocket() << " broadcast " << "\"" << msg << "\"" << std::endl;
		}
	}
}
