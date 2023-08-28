#include "pch.hpp"

namespace Engine
{
	Session::Session() : m_RecvBuffer(BUFFER_SIZE)
	{

	}

	void Session::Send(const char* msg)
	{
		WRITE_LOCK;

		const SOCKET socket = GetSocket();

		char* sendBuffer = GetSendBuffer();
		memcpy(sendBuffer, msg, strlen(msg));
		
		WSABUF sendWSABuf;
		sendWSABuf.buf = sendBuffer;
		sendWSABuf.len = static_cast<ULONG>(strlen(sendBuffer));
		
		DWORD recvLen = 0;
		DWORD flags = 0;
		
		IOCPEvent* iocpEvent = xnew<IOCPEvent>();
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
		
		WSABUF recvWSABuf;
		recvWSABuf.buf = reinterpret_cast<char*>(m_RecvBuffer.WritePos());
		recvWSABuf.len = m_RecvBuffer.FreeSize();
		
		DWORD recvLen = 0;
		DWORD flags = 0;
		
		IOCPEvent* iocpEvent = xnew<IOCPEvent>();
		iocpEvent->SetIOType(IO_TYPE::SERVER_RECV);
		if (::WSARecv(socket, &recvWSABuf, 1, &recvLen, &flags, iocpEvent, nullptr))
		{
			std::cout << "Session::Recv()" << std::endl;
			std::cout << "::WSARecv WSAGetLastError: " << ::WSAGetLastError() << std::endl;
		}
	}
}