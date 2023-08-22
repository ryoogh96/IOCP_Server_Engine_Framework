#include "pch.hpp"

namespace Engine
{
	ClientSocketManager::ClientSocketManager()
	{
		Initialize();
	}

	ClientSocketManager::~ClientSocketManager()
	{
		if (m_socket != INVALID_SOCKET)
			::closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		::WSACleanup();

		delete m_iocpManager;
		delete m_Connector;
	}

	void ClientSocketManager::Initialize()
	{
		WSADATA wsaData;
		const int wsaStartup = ::WSAStartup(MAKEWORD(2, 2), OUT & wsaData);
		if (wsaStartup != 0)
		{
			std::cout << "ClientSocketManager::ClientSocketManager()" << std::endl;
			std::cout << "::WSAStartup Error: " << wsaStartup << std::endl;
			ASSERT_CRASH(wsaStartup == 0);
		}

		m_socket = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_socket == INVALID_SOCKET)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::WSASocketW WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(m_socket != INVALID_SOCKET);
		}

		SOCKADDR_IN sockAddr;
		ZeroMemory(&sockAddr, sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		// use remain any port
		sockAddr.sin_port = ::htons(0);
		sockAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);

		const int bind = ::bind(m_socket, reinterpret_cast<const SOCKADDR*>(&sockAddr), sizeof(sockAddr)) != SOCKET_ERROR;
		if (bind == SOCKET_ERROR)
		{
			std::cout << "ServerSocketManager::ServerSocketManager()" << std::endl;
			std::cout << "::bind WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(bind != SOCKET_ERROR);
		}

		m_iocpManager = new IOCPManager();
		m_iocpManager->AttachSocketToIOCP(m_socket);

		DWORD byte = 0;
		GUID connectExGUID = WSAID_CONNECTEX;
		LPFN_CONNECTEX fpConnectEx;
		if (SOCKET_ERROR == ::WSAIoctl(m_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &connectExGUID, sizeof(connectExGUID), &fpConnectEx, sizeof(fpConnectEx), &byte, nullptr, nullptr))
		{
			const int lastError = ::WSAGetLastError();
			if (lastError != WSA_IO_PENDING)
			{
				std::cout << "ClientSocketManager::ClientSocketManager()" << std::endl;
				std::cout << "::WSAIoctl WSAGetLastError: " << lastError << std::endl;;
				return;
			}
		}

		IN_ADDR address;
		::InetPtonW(AF_INET, SERVER_IP, &address);
		sockAddr.sin_port = ::htons(DEFAULT_PORT);
		sockAddr.sin_addr = address;
		m_Connector = new Connector();
		if (FALSE == fpConnectEx(m_socket, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR), m_Connector->GetConnectorBuf(), 0, &byte, m_Connector))
		{
			const int lastError = ::WSAGetLastError();
			if (lastError != WSA_IO_PENDING)
			{
				std::cout << "ClientSocketManager::ClientSocketManager()" << std::endl;
				std::cout << "fpConnectEx WSAGetLastError: " << lastError << std::endl;;
				//return;
			}
		}
	}

	void ClientSocketManager::Send(char sendBuffer[])
	{
		if (sizeof(sendBuffer) > 1024)
		{
			std::cout << "now sendBuffer only support 1024 size" << std::endl;
			return;
		}

		DWORD numOfBytes = sizeof(sendBuffer);
		DWORD flags = 0;
		IOCPEvent* iocpEvent = xnew<IOCPEvent>();
		iocpEvent->SetIOType(IO_TYPE::CLIENT_SEND);
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer);
		wsaBuf.len = strlen(sendBuffer);

		while (true)
		{
			if (::WSASend(m_socket, &wsaBuf, 1, &numOfBytes, flags, iocpEvent, nullptr) != SOCKET_ERROR)
			{
				// Success
				return;
			}
			else
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					continue;
				}
				else {
					std::cout << "ClientSocketManager::send(char sendBuffer[])" << std::endl;
					std::cout << "::WSASend WSAGetLastError: " << ::WSAGetLastError() << std::endl;
				}
			}
		}

		//delete sendWSABuf;
	}

	void ClientSocketManager::Recv()
	{
		WSABUF wsaBuf;
		wsaBuf.buf = m_recvBuffer;
		wsaBuf.len = MAX_BUF_SIZE;
		DWORD recvLen = 0;
		DWORD flags = 0;
		IOCPEvent* iocpEvent = xnew<IOCPEvent>();
		iocpEvent->SetIOType(IO_TYPE::CLIENT_RECV);

		const int wsaResult = ::WSARecv(m_socket, &wsaBuf, 1, &recvLen, &flags, iocpEvent, nullptr);
		if (wsaResult == SOCKET_ERROR)
		{
			if (::WSAGetLastError() != WSA_IO_PENDING)
			{
				std::cout << "ClientSocketManager::recv()" << std::endl;
				std::cout << "WSARecv WSAGetLastError: " << ::WSAGetLastError() << std::endl;
				return;
			}
			else
			{
				if (strlen(m_recvBuffer) > 0) {
					std::cout << "m_recvBuffer: " << m_recvBuffer << std::endl;
				}
				return;
			}
		}
	}
}