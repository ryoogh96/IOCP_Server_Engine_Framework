#include "pch.hpp"
#include "NetAddress.hpp"

namespace Engine
{
	NetAddress::NetAddress(SOCKADDR_IN sockAddr) : m_SockAddr(sockAddr)
	{

	}

	NetAddress::NetAddress(std::wstring ip, uint16 port)
	{
		ZeroMemory(&m_SockAddr, sizeof(m_SockAddr));
		m_SockAddr.sin_family = AF_INET;
		m_SockAddr.sin_addr = Ip2Address(ip.c_str());
		m_SockAddr.sin_port = ::htons(port);
	}

	std::wstring NetAddress::GetIpAddress()
	{
		WCHAR buffer[100];
		::InetNtopW(AF_INET, &m_SockAddr.sin_addr, buffer, len32(buffer));
		return std::wstring(buffer);
	}

	IN_ADDR NetAddress::Ip2Address(const WCHAR* ip)
	{
		IN_ADDR address;
		::InetPtonW(AF_INET, ip, &address);
		return address;
	}
}