#include "pch.hpp"

#include "ServerEnginePCH.hpp"

namespace Engine
{
	void Session::send(const char* msg)
	{
		memcpy(m_sendBuffer, msg, strlen(msg));

		WSABUF sendWSABuf;
		sendWSABuf.buf = m_sendBuffer;
		sendWSABuf.len = sizeof(m_sendBuffer);
		DWORD recvLen = 0;
		DWORD flags = 0;
		ExtendOverlapped* extendOverlapped = new ExtendOverlapped();
		extendOverlapped->type = IO_TYPE::WRITE;
		if (::WSASend(m_socket, &sendWSABuf, 1, &recvLen, flags, (LPWSAOVERLAPPED)extendOverlapped, nullptr) == SOCKET_ERROR)
		{
			std::cout << "Session::send()" << std::endl;
			std::cout << "::WSASend WSAGetLastError: " << ::WSAGetLastError() << std::endl;
		}
	}

	void Session::recv()
	{
		WSABUF recvWSABuf;
		recvWSABuf.buf = m_recvBuffer;
		recvWSABuf.len = MAX_BUF_SIZE;
		DWORD recvLen = 0;
		DWORD flags = 0;
		ExtendOverlapped* extendOverlapped = new ExtendOverlapped();
		extendOverlapped->type = IO_TYPE::READ;
		::WSARecv(m_socket, &recvWSABuf, 1, &recvLen, &flags, (LPWSAOVERLAPPED)extendOverlapped, nullptr);
	}
}