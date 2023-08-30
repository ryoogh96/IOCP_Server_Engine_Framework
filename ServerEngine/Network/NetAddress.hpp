#pragma once

namespace Engine
{
	class NetAddress
	{
	public:
		NetAddress() = default;
		NetAddress(SOCKADDR_IN sockAddr);
		NetAddress(std::wstring ip, uint16 port);

		SOCKADDR_IN&	GetSockAddr() { return m_SockAddr; }
		std::wstring	GetIpAddress();
		uint16			GetPort() { return ::ntohs(m_SockAddr.sin_port); }

	public:
		static IN_ADDR	Ip2Address(const WCHAR* ip);

	private:
		SOCKADDR_IN	m_SockAddr = {};
	};
}