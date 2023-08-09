#pragma once

namespace Engine
{
	class ClientSocketManager
	{
	public:
		ClientSocketManager();
		~ClientSocketManager();
		void send(char sendBuffer[]);
		void recv();

		char* getRecvBuffer() { return m_recvBuffer; }
		void setRecvBuffer(char buf[]) { memcpy(m_recvBuffer, buf, sizeof(buf)); }

	private:
		SOCKET m_socket;
		char m_recvBuffer[MAX_BUF_SIZE] = {};
	};
}