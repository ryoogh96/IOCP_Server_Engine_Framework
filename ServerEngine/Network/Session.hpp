#pragma once

namespace Engine
{
	class Session
	{
	public:
		Session() {};
		~Session() {};

		void Send(const char* msg);
		void Recv();

		const SOCKET GetSocket() const { return m_Socket; }
		void SetSocket(const SOCKET socket) { m_Socket = socket; }
		char* GetSendBuffer() { return m_SendBuffer; }
		void SetSendBuffer(char buf[]) { memcpy(m_SendBuffer, buf, sizeof(buf)); }
		char* GetRecvBuffer() { return m_RecvBuffer; }
		void SetRecvBuffer(char buf[]) { memcpy(m_RecvBuffer, buf, sizeof(buf)); }

	private:
		USE_LOCK;

	private:
		SOCKET m_Socket = INVALID_SOCKET;
		char m_SendBuffer[MAX_BUF_SIZE] = {};
		char m_RecvBuffer[MAX_BUF_SIZE] = {};
	};
}

