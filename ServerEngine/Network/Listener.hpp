#pragma once

namespace Engine
{
	class Listener
	{
	public:
		Listener() {};
		~Listener();

		const SOCKET getListenSocket() { return m_ListenSocket; }
		void setListenSocket(const SOCKET socket) { m_ListenSocket = socket; }

		void CreateAcceptSocket(const HANDLE IOCPHandle, std::map<SOCKET, Session*> sessionMap) const;

	private:
		SOCKET m_ListenSocket = INVALID_SOCKET;
	};
}