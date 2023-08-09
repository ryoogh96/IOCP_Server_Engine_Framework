#pragma once

namespace Engine
{
	class ClientSocketManager
	{
	public:
		ClientSocketManager();
		~ClientSocketManager();
		void Send(char sendBuffer[]);
		void Recv();

		char* GetRecvBuffer() { return m_recvBuffer; }
		void SetRecvBuffer(char buf[]) { memcpy(m_recvBuffer, buf, sizeof(buf)); }

	private:
		SOCKET m_socket;
		char m_recvBuffer[MAX_BUF_SIZE] = {};
	};
}