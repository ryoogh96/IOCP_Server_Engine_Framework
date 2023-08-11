#include "pch.hpp"

#include "ServerEnginePCH.hpp"

namespace Engine
{
	ClientSocketManager::ClientSocketManager()
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

		SOCKADDR_IN serverAddr;
		ZeroMemory(&serverAddr, sizeof(serverAddr));

		std::string msg;
		std::string* socketBuf = new std::string;
		IN_ADDR address;
		::InetPtonW(AF_INET, SERVER_IP, &address);
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = ::htons(DEFAULT_PORT);
		serverAddr.sin_addr = address;
		DWORD numOfBytes = 0;
		ASSERT_CRASH(::WSAConnect(m_socket, (sockaddr*)&serverAddr, sizeof(sockaddr), nullptr, nullptr, nullptr, nullptr) != SOCKET_ERROR);
	}

	ClientSocketManager::~ClientSocketManager()
	{
		if (m_socket != INVALID_SOCKET)
			::closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		::WSACleanup();
	}

	void ClientSocketManager::Send(char sendBuffer[])
	{
		if (sizeof(sendBuffer) > 1024)
		{
			std::cout << "now sendBuffer only support 1024 size" << std::endl;
			return;
		}

		//WSABUF* sendWSABuf = new WSABUF();
		//memcpy(sendWSABuf->buf, sendBuffer, sizeof(sendBuffer));
		//sendWSABuf->len = sizeof(sendBuffer);
		DWORD numOfBytes = sizeof(sendBuffer);
		DWORD flags = 0;
		ExtendOverlapped* extendOverlapped = new ExtendOverlapped();
		extendOverlapped->m_type = IO_TYPE::WRITE;
		WSABUF wsaBuf;
		wsaBuf.buf = (CHAR*)sendBuffer;
		wsaBuf.len = sizeof(sendBuffer);

		while (true)
		{
			if (::WSASend(m_socket, &wsaBuf, 1, &numOfBytes, flags, (LPWSAOVERLAPPED)&extendOverlapped, nullptr) != SOCKET_ERROR)
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
		WSAOVERLAPPED overlapped = {};

		const int wsaResult = ::WSARecv(m_socket, &wsaBuf, 1, &recvLen, &flags, &overlapped, nullptr);
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