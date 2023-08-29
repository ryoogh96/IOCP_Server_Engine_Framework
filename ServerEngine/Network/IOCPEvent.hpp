#pragma once

namespace Engine
{
	enum class EVENT_TYPE : uint8
	{
		NONE,
		CONNECT,
		DISCONNECT,
		SEND,
		RECV,
		ACCEPT,
	};

	class IOCPEvent : public WSAOVERLAPPED
	{
	public:
		IOCPEvent(EVENT_TYPE eventType) : m_EventType(eventType)
		{
			Internal = 0;
			InternalHigh = 0;
			Offset = 0;
			OffsetHigh = 0;
			hEvent = nullptr;
		}

		void SetIOType(EVENT_TYPE type) { m_EventType = type; }
		const EVENT_TYPE GetEventType() const { return m_EventType; }
		void SetSocket(const SOCKET socket) { m_Socket = socket; }
		const SOCKET GetSocket() const { return m_Socket; }
		void SetBuffer(char buf[]) { memcpy(m_Buf, buf, sizeof(buf)); }
		char* GetBuffer() { return m_Buf; }
		void SetOwner(IOCPObjectRef owner) { m_Owner = owner; }
		IOCPObjectRef GetOwner() { return m_Owner; }

	private:
		EVENT_TYPE	m_EventType = EVENT_TYPE::NONE;
		SOCKET m_Socket = INVALID_SOCKET;
		char m_Buf[512] = { 0, };
		IOCPObjectRef m_Owner;
	};

	class SendEvent : public IOCPEvent
	{
	public:
		SendEvent() : IOCPEvent(EVENT_TYPE::SEND) {}

		Vector<SendBufferRef> m_SendBuffers;
	};
}