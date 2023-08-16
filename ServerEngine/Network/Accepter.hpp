#pragma once

#include "pch.hpp"

#include "ServerEnginePCH.hpp"

namespace Engine
{
	class Accepter : public WSAOVERLAPPED
	{
	public:
		Accepter()
		{
			Internal = 0;
			InternalHigh = 0;
			Offset = 0;
			OffsetHigh = 0;
			hEvent = nullptr;
		}
		
		const IO_TYPE GetIOType() const { return m_IO_Type; }
		const SOCKET GetSocket() const { return m_Socket; }
		void SetSocket(const SOCKET socket) { m_Socket = socket; }
		void SetAccepterBuf(char buf[]) { memcpy(m_AcceptBuf, buf, sizeof(buf)); }
		char* GetAccepterBuf() { return m_AcceptBuf; }

	private:
		IO_TYPE	m_IO_Type = IO_TYPE::SERVER_ACCEPT;
		SOCKET m_Socket = INVALID_SOCKET;
		char m_AcceptBuf[512] = { 0, };
	};
}