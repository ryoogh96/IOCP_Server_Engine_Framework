#pragma once

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
		
		const EVENT_TYPE GetEventType() const { return m_IO_Type; }
		const SOCKET GetSocket() const { return m_Socket; }
		void SetSocket(const SOCKET socket) { m_Socket = socket; }
		void SetAccepterBuf(char buf[]) { memcpy(m_AcceptBuf, buf, sizeof(buf)); }
		char* GetAccepterBuf() { return m_AcceptBuf; }

	private:
		EVENT_TYPE	m_IO_Type = EVENT_TYPE::ACCEPT;
		SOCKET m_Socket = INVALID_SOCKET;
		char m_AcceptBuf[512] = { 0, };
	};
}