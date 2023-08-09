#pragma once

#include "ServerEnginePCH.hpp"

namespace Engine
{
	class Session
	{
	public:
		Session() {};
		~Session() {};

		void Send(const char* msg);
		void Recv();

		const SOCKET GetSocket() const { return m_socket; }
		void SetSocket(const SOCKET socket) { m_socket = socket; }

		char* GetSendBuffer() { return m_sendBuffer; }
		void SetSendBuffer(char buf[]) { memcpy(m_sendBuffer, buf, sizeof(buf)); }

		char* GetRecvBuffer() { return m_recvBuffer; }
		void SetRecvBuffer(char buf[]) { memcpy(m_recvBuffer, buf, sizeof(buf)); }

	private:
		SOCKET m_socket = INVALID_SOCKET;
		char m_sendBuffer[MAX_BUF_SIZE] = {};
		char m_recvBuffer[MAX_BUF_SIZE] = {};
	};
}

