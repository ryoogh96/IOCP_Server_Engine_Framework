#include "pch.hpp"

#include "ServerSocketManager.hpp"

namespace Engine
{
	ServerSocketManager::ServerSocketManager()
	{
		Initialize();
		//for (uint8 i = 0; i < MAX_CLIENT_SOCKET_POOL; ++i)
		//{
		//	acceptClientSocket();
		//}
	}

	ServerSocketManager::~ServerSocketManager()
	{
		if (m_ListenSocket != INVALID_SOCKET)
			::closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;

		for (const auto sessionMap : m_sessionMap)
		{
			const SOCKET socket = sessionMap.second->GetSocket();
			if (socket != INVALID_SOCKET)
				::closesocket(socket);
			sessionMap.second->SetSocket(INVALID_SOCKET);

			Session* session = sessionMap.second;
			delete session;
		}

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

		m_ListenSocket = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_ListenSocket == INVALID_SOCKET)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::WSASocketW WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(m_ListenSocket != INVALID_SOCKET);
		}

		constexpr int opt = 1;
		const int sockopt = ::setsockopt(m_ListenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(int));
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

		const int bind = ::bind(m_ListenSocket, reinterpret_cast<const SOCKADDR*>(&sockAddr), sizeof(sockAddr)) != SOCKET_ERROR;
		if (bind == SOCKET_ERROR)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::bind WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(bind != SOCKET_ERROR);
		}
		const int listen = ::listen(m_ListenSocket, SOMAXCONN);
		if (listen == SOCKET_ERROR)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::listen WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(listen != SOCKET_ERROR);
		}

		m_Connector = new Connector();
		m_iocpManager = new IOCPManager();
	}

	void ServerSocketManager::AcceptClientSocket()
	{
		const SOCKET clientSocket = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cout << "ServerSocketManager::acceptClientConnect()" << std::endl;
			std::cout << "::WSASocketW WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(clientSocket != INVALID_SOCKET);
		}

		Session* session = new Session();
		session->SetSocket(clientSocket);
		if (FALSE == ::AcceptEx(m_ListenSocket, session->GetSocket(), m_Connector->GetConnectorBuf(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, m_Connector->GetBytesReceived(), m_Connector))
		{
			const int lastError = ::WSAGetLastError();
			if (lastError != WSA_IO_PENDING)
			{
				std::cout << "ServerSocketManager::acceptClientConnect()" << std::endl;
				std::cout << "::AcceptEx WSAGetLastError: " << lastError << std::endl;;
				AcceptClientSocket();
				return;
			}
		}

		m_iocpManager->AttachSocketToIoCompletionPort(session->GetSocket(), session);

		m_sessionMap.insert(make_pair(session->GetSocket(), session));

		// TODO: move msg to real connection logic
		//std::cout << "session->socket: " << session->getSocket() << " has been connected" << std::endl;

		WSABUF recvWSABuf;
		ExtendOverlapped* extendOverlapped = new ExtendOverlapped();
		recvWSABuf.buf = reinterpret_cast<char*>(session->GetRecvBuffer());
		recvWSABuf.len = MAX_BUF_SIZE;
		DWORD recvLen = 0;
		DWORD flags = 0;
		extendOverlapped->type = IO_TYPE::READ;
		if (SOCKET_ERROR == ::WSARecv(session->GetSocket(), &recvWSABuf, 1, &recvLen, &flags, (LPWSAOVERLAPPED)extendOverlapped, nullptr))
		{
			const int lastError = ::WSAGetLastError();
			if (lastError != WSA_IO_PENDING && lastError != WSAENOTCONN)
			{
				std::cout << "ServerSocketManager::acceptClientConnect()" << std::endl;
				std::cout << "::WSARecv WSAGetLastError: " << lastError << std::endl;
				ASSERT_CRASH(lastError == WSA_IO_PENDING || lastError == WSAENOTCONN);
				return;
			}
		}
	}

	void ServerSocketManager::Broadcast()
	{
		char msg[] = "message broadcasting...";
		for (const auto sessionMap : m_sessionMap)
		{
			Session* session = sessionMap.second;
			session->Send(msg);

			std::cout << "session->socket: " << session->GetSocket() << " broadcast " << "\"" << msg << "\"" << std::endl;
		}
	}
}
