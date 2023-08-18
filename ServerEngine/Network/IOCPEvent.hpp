#pragma once

#include "pch.hpp"

#include "Core/ServerEnginePCH.hpp"

namespace Engine
{
	enum class IO_TYPE
	{
		NONE,
		CLIENT_CONNECT,
		CLIENT_DISCONNECT,
		CLIENT_SEND,
		CLIENT_RECV,
		SERVER_ACCEPT,
		SERVER_SEND,
		SERVER_RECV,
	};

	class IOCPEvent : public WSAOVERLAPPED
	{
	public:
		IOCPEvent()
		{
			Internal = 0;
			InternalHigh = 0;
			Offset = 0;
			OffsetHigh = 0;
			hEvent = nullptr;
		}

		void SetIOType(IO_TYPE type) { m_IO_Type = type; }
		const IO_TYPE GetIOType() const { return m_IO_Type; }
		void SetSocket(const SOCKET socket) { m_Socket = socket; }
		const SOCKET GetSocket() const { return m_Socket; }
		void SetBuffer(char buf[]) { memcpy(m_Buf, buf, sizeof(buf)); }

	private:
		IO_TYPE	m_IO_Type = IO_TYPE::NONE;
		SOCKET m_Socket = INVALID_SOCKET;
		char m_Buf[512] = { 0, };
	};
}