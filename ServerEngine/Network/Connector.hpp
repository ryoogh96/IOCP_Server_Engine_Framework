#pragma once

namespace Engine
{
	class Connector : public WSAOVERLAPPED
	{
	public:
		Connector()
		{
			Internal = 0;
			InternalHigh = 0;
			Offset = 0;
			OffsetHigh = 0;
			hEvent = nullptr;
		}
		~Connector() {}

		const IO_TYPE GetIOType() const { return m_IO_Type; }
		void SetConnectorBuf(char buf[]) { memcpy(m_ConnectorBuf, buf, sizeof(buf)); }
		char* GetConnectorBuf() { return m_ConnectorBuf; }
		DWORD SetBytesReceived(const DWORD bytesReceived) { m_BytesReceived = bytesReceived; }
		DWORD GetBytesReceived() const { return m_BytesReceived; }

	private:
		IO_TYPE	m_IO_Type = IO_TYPE::CLIENT_CONNECT;
		char m_ConnectorBuf[MAX_BUF_SIZE] = { 0, };
		DWORD m_BytesReceived = 0;
	};
}