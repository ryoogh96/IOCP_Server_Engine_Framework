#pragma once

namespace Engine
{
	enum class EVENT_TYPE : uint8
	{
		NONE,
		CONNECT,
		DISCONNECT,
		ACCEPT,
		SEND,
		RECV,
	};

	class IOCPEvent : public WSAOVERLAPPED
	{
	public:
		IOCPEvent(EVENT_TYPE type) : m_EventType(type) { Init(); };

		void			Init() { OVERLAPPED::hEvent = 0; OVERLAPPED::Internal = 0; OVERLAPPED::InternalHigh = 0; OVERLAPPED::Offset = 0; OVERLAPPED::OffsetHigh = 0; }

	public:
		EVENT_TYPE		m_EventType;
		IOCPObjectRef	m_Owner;
	};

	class ConnectEvent : public IOCPEvent
	{
	public:
		ConnectEvent() : IOCPEvent(EVENT_TYPE::CONNECT) { }
	};

	class DisconnectEvent : public IOCPEvent
	{
	public:
		DisconnectEvent() : IOCPEvent(EVENT_TYPE::DISCONNECT) { }
	};

	class AcceptEvent : public IOCPEvent
	{
	public:
		AcceptEvent() : IOCPEvent(EVENT_TYPE::ACCEPT) { }

	public:
		SessionRef	session = nullptr;
	};

	class RecvEvent : public IOCPEvent
	{
	public:
		RecvEvent() : IOCPEvent(EVENT_TYPE::RECV) { }
	};

	class SendEvent : public IOCPEvent
	{
	public:
		SendEvent() : IOCPEvent(EVENT_TYPE::SEND) {}

		Vector<SendBufferRef> m_SendBuffers;
	};
}