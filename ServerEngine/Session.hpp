#pragma once

#include "ServerEnginePCH.hpp"

namespace Engine
{
	class Session
	{
	public:
		Session() {};
		~Session() {};

		void send(const char* msg);
		void recv();

		const SOCKET getSocket() const { return m_socket; }
		void setSocket(const SOCKET socket) { m_socket = socket; }

		char* getSendBuffer() { return m_sendBuffer; }
		void setSendBuffer(char buf[]) { memcpy(m_sendBuffer, buf, sizeof(buf)); }

		char* getRecvBuffer() { return m_recvBuffer; }
		void setRecvBuffer(char buf[]) { memcpy(m_recvBuffer, buf, sizeof(buf)); }

	private:
		SOCKET m_socket = INVALID_SOCKET;
		char m_sendBuffer[MAX_BUF_SIZE] = {};
		char m_recvBuffer[MAX_BUF_SIZE] = {};
	};
}

