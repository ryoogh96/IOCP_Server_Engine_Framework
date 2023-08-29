#pragma once

namespace Engine
{
	class IOCPObject : public std::enable_shared_from_this<IOCPObject>
	{
	public:
		IOCPObject() {}
		~IOCPObject() {}
	};

	class Session : public IOCPObject
	{
		enum
		{
			BUFFER_SIZE = 0x10000, // 64KB
		};

	public:
		Session();
		~Session() { std::cout << "~Session()" << std::endl; };

		void Send(SendBufferRef sendBuffer);
		void Recv();

		const SOCKET GetSocket() const { return m_Socket; }
		void SetSocket(const SOCKET socket) { m_Socket = socket; }
		char* GetSendBuffer() { return m_SendBuffer; }
		void SetSendBuffer(char buf[]) { memcpy(m_SendBuffer, buf, sizeof(buf)); }
		RecvBuffer  GetRecvBuffer() { return m_RecvBuffer; }
		void SetRecvBuffer(RecvBuffer& recvBuffer) { m_RecvBuffer = recvBuffer; }

	private:
		void HandleError(int32 errorCode);

	private:
		USE_LOCK;

		RecvBuffer				m_RecvBuffer;

		Queue<SendBufferRef>	m_SendQueue;
		Atomic<bool>			m_SendRegistered = false;

	private:
		SendEvent m_SendEvent;

	private:
		SOCKET m_Socket = INVALID_SOCKET;
		char m_SendBuffer[MAX_BUF_SIZE] = {};
	};
}

