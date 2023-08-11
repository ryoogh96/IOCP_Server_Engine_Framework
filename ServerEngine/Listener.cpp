#include "pch.hpp"

#include "ServerEnginePCH.hpp"

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
		ExtendOverlapped* extendOverlapped = new ExtendOverlapped();
		extendOverlapped->m_type = IO_TYPE::ACCEPT;
		extendOverlapped->m_acceptSocket = acceptSocket;
		if (FALSE == ::AcceptEx(m_ListenSocket, acceptSocket, &extendOverlapped->m_acceptBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, reinterpret_cast<const LPOVERLAPPED>(extendOverlapped)))
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