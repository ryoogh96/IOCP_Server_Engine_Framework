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

		void SetSocket(const SOCKET socket) { m_socket = socket; }
		const SOCKET GetSocket() { return m_socket; }
		char* GetRecvBuffer() { return m_recvBuffer; }
		void SetRecvBuffer(char buf[]) { memcpy(m_recvBuffer, buf, sizeof(buf)); }

	private:
		SOCKET m_socket = INVALID_SOCKET;
		char m_recvBuffer[MAX_BUF_SIZE] = {};
		IOCPManager* m_iocpManager = nullptr;
		Connector* m_Connector = nullptr;

		void Initialize();
	};
}