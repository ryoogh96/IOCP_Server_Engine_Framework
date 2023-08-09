#pragma once

#include "ServerEnginePCH.hpp"

namespace Engine
{
	class Connector : public OVERLAPPED
	{
	public:
		Connector() {}
		~Connector() {}
		char* getConnectorBuf() { return m_connectorBuf; }
		DWORD* getBytesReceived() { return m_bytesReceived; }

	private:
		char m_connectorBuf[MAX_BUF_SIZE] = { 0, };
		DWORD* m_bytesReceived = 0;
	};
}