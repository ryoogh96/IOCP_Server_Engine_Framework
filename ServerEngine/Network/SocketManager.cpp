#include "pch.hpp"
#include "SocketManager.hpp"

namespace Engine
{
	LPFN_CONNECTEX		SocketManager::ConnectEx = nullptr;
	LPFN_DISCONNECTEX	SocketManager::DisconnectEx = nullptr;
	LPFN_ACCEPTEX		SocketManager::AcceptEx = nullptr;

	void SocketManager::Initialize()
	{
		WSADATA wsaData;
		ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);

		SOCKET dummySocket = CreateSocket();
		ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
		ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
		ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
		Close(dummySocket);
	}

	void SocketManager::Clear()
	{
		::WSACleanup();
	}

	bool SocketManager::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
	{
		DWORD bytes = 0;
		return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
	}

	SOCKET SocketManager::CreateSocket()
	{
		return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	}

	bool SocketManager::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
	{
		LINGER option;
		option.l_onoff = onoff;
		option.l_linger = linger;
		return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
	}

	bool SocketManager::SetReuseAddress(SOCKET socket, bool flag)
	{
		return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
	}

	bool SocketManager::SetRecvBufferSize(SOCKET socket, int32 size)
	{
		return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
	}

	bool SocketManager::SetSendBufferSize(SOCKET socket, int32 size)
	{
		return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
	}

	bool SocketManager::SetTcpNoDelay(SOCKET socket, bool flag)
	{
		return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
	}

	bool SocketManager::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
	{
		return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
	}

	bool SocketManager::Bind(SOCKET socket, NetAddress netAddr)
	{
		return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
	}

	bool SocketManager::BindAnyAddress(SOCKET socket, uint16 port)
	{
		SOCKADDR_IN myAddress;
		myAddress.sin_family = AF_INET;
		myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
		myAddress.sin_port = ::htons(port);

		return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
	}

	bool SocketManager::Listen(SOCKET socket, int32 backlog)
	{
		return SOCKET_ERROR != ::listen(socket, backlog);
	}

	void SocketManager::Close(SOCKET& socket)
	{
		if (socket != INVALID_SOCKET)
			::closesocket(socket);
		socket = INVALID_SOCKET;
	}
}