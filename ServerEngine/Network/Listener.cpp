#include "pch.hpp"

#include "Core/ServerEnginePCH.hpp"

namespace Engine
{
	Listener::~Listener()
	{
		if (m_ListenSocket != INVALID_SOCKET)
			::closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
	}


	void Listener::CreateAcceptSocket(const HANDLE IOCPHandle, std::map<SOCKET, Session*> sessionMap) const
	{
		const SOCKET acceptSocket = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (acceptSocket == INVALID_SOCKET)
		{
			std::cout << "Listener::CreateAcceptSocket()" << std::endl;
			std::cout << "::WSASocketW WSAGetLastError: " << ::WSAGetLastError() << std::endl;
			ASSERT_CRASH(acceptSocket != INVALID_SOCKET);
		}


		DWORD bytesReceived = 0;
		Accepter* accepter = new Accepter();
		accepter->SetSocket(acceptSocket);
		if (FALSE == ::AcceptEx(m_ListenSocket, acceptSocket, accepter->GetAccepterBuf(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, accepter))
		{
			const int lastError = ::WSAGetLastError();
			if (lastError != WSA_IO_PENDING)
			{
				std::cout << "Listener::CreateAcceptSocket()" << std::endl;
				std::cout << "::AcceptEx WSAGetLastError: " << lastError << std::endl;;
				return;
			}
		}
	}
}