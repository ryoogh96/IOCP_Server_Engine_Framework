#pragma once

namespace Engine
{
	class Session
	{
		enum
		{
			BUFFER_SIZE = 0x10000, // 64KB
		};

	public:
		Session();
		~Session() { std::cout << "~Session()" << std::endl; };

		void Send(const char* msg);
		void Recv();

		const SOCKET GetSocket() const { return m_Socket; }
		void SetSocket(const SOCKET socket) { m_Socket = socket; }
		char* GetSendBuffer() { return m_SendBuffer; }
		void SetSendBuffer(char buf[]) { memcpy(m_SendBuffer, buf, sizeof(buf)); }
		RecvBuffer  GetRecvBuffer() { return m_RecvBuffer; }
		void SetRecvBuffer(RecvBuffer& recvBuffer) { m_RecvBuffer = recvBuffer; }

	private:
		USE_LOCK;

		RecvBuffer m_RecvBuffer;

	private:
		SOCKET m_Socket = INVALID_SOCKET;
		char m_SendBuffer[MAX_BUF_SIZE] = {};
	};
}

