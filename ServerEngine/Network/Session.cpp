#include "pch.hpp"

#include "Core/ServerEnginePCH.hpp"

namespace Engine
{
	void Session::Send(const char* msg)
	{
		const SOCKET socket = GetSocket();

		char* sendBuffer = GetSendBuffer();
		memcpy(sendBuffer, msg, strlen(msg));
		
		WSABUF sendWSABuf;
		sendWSABuf.buf = sendBuffer;
		sendWSABuf.len = strlen(sendBuffer);
		
		DWORD recvLen = 0;
		DWORD flags = 0;
		
		IOCPEvent* iocpEvent = new IOCPEvent();
		iocpEvent->SetIOType(IO_TYPE::SERVER_SEND);
		if (::WSASend(socket, &sendWSABuf, 1, &recvLen, flags, iocpEvent, nullptr) == SOCKET_ERROR)
		{
			std::cout << "Session::Send()" << std::endl;
			std::cout << "::WSASend WSAGetLastError: " << ::WSAGetLastError() << std::endl;
		}
	}

	void Session::Recv()
	{
		const SOCKET socket = GetSocket();
		
		char* recvBuffer = GetRecvBuffer();
		
		WSABUF recvWSABuf;
		recvWSABuf.buf = recvBuffer;
		recvWSABuf.len = MAX_BUF_SIZE;
		
		DWORD recvLen = 0;
		DWORD flags = 0;
		
		IOCPEvent* iocpEvent = new IOCPEvent();
		iocpEvent->SetIOType(IO_TYPE::SERVER_RECV);
		if (::WSARecv(socket, &recvWSABuf, 1, &recvLen, &flags, iocpEvent, nullptr))
		{
			std::cout << "Session::Recv()" << std::endl;
			std::cout << "::WSARecv WSAGetLastError: " << ::WSAGetLastError() << std::endl;
		}
	}
}