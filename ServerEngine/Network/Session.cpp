#include "pch.hpp"
#include "Session.hpp"

namespace Engine
{
	Session::Session() : m_RecvBuffer(BUFFER_SIZE)
	{
		m_Socket = SocketManager::CreateSocket();
	}

	Session::~Session()
	{
		SocketManager::Close(m_Socket);
	}

	void Session::Send(SendBufferRef sendBuffer)
	{
		if (IsConnected() == false)
			return;

		bool registerSend = false;
		
		{
			WRITE_LOCK;

			m_SendQueue.push(sendBuffer);

			if (m_SendRegistered.exchange(true) == false)
				registerSend = true;
		}

		if (registerSend)
			RegisterSend();
	}

	bool Session::Connect()
	{
		return RegisterConnect();
	}

	void Session::Disconnect(const WCHAR* cause)
	{
		if (m_Connected.exchange(false) == false)
			return;

		std::wcout << "Disconnect : " << cause << std::endl;

		RegisterDisconnect();
	}

	HANDLE Session::GetHandle()
	{
		return reinterpret_cast<HANDLE>(m_Socket);
	}

	void Session::Dispatch(IOCPEvent* iocpEvent, int32 numOfBytes)
	{
		switch (iocpEvent->m_EventType)
		{
		case EVENT_TYPE::CONNECT:
			ProcessConnect();
			break;
		case EVENT_TYPE::DISCONNECT:
			ProcessDisconnect();
			break;
		case EVENT_TYPE::RECV:
			ProcessRecv(numOfBytes);
			break;
		case EVENT_TYPE::SEND:
			ProcessSend(numOfBytes);
			break;
		default:
			break;
		}
	}

	bool Session::RegisterConnect()
	{
		if (IsConnected())
			return false;

		if (GetService()->GetServiceType() != ServiceType::Client)
			return false;

		if (SocketManager::SetReuseAddress(m_Socket, true) == false)
			return false;

		if (SocketManager::BindAnyAddress(m_Socket, 0 /* use any remain port */) == false)
			return false;

		m_ConnectEvent.Init();
		m_ConnectEvent.m_Owner = shared_from_this(); // ADD_REF

		DWORD numOfBytes = 0;
		SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
		if (false == SocketManager::ConnectEx(m_Socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &m_ConnectEvent))
		{
			int32 errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				m_ConnectEvent.m_Owner = nullptr; // RELEASE_REF
				return false;
			}
		}

		return true;
	}

	bool Session::RegisterDisconnect()
	{
		m_DisconnectEvent.Init();
		m_DisconnectEvent.m_Owner = shared_from_this(); // ADD_REF

		if (false == SocketManager::DisconnectEx(m_Socket, &m_DisconnectEvent, TF_REUSE_SOCKET, 0))
		{
			int32 errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				m_DisconnectEvent.m_Owner = nullptr; // RELEASE_REF
				return false;
			}
		}

		return true;
	}

	void Session::RegisterRecv()
	{
		if (IsConnected() == false)
			return;

		m_RecvEvent.Init();
		m_RecvEvent.m_Owner = shared_from_this(); // ADD_REF

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(m_RecvBuffer.WritePos());
		wsaBuf.len = m_RecvBuffer.FreeSize();

		DWORD numOfBytes = 0;
		DWORD flags = 0;
		if (SOCKET_ERROR == ::WSARecv(m_Socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &m_RecvEvent, nullptr))
		{
			int32 errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				HandleError(errorCode);
				m_RecvEvent.m_Owner = nullptr; // RELEASE_REF
			}
		}
	}

	void Session::RegisterSend()
	{
		if (IsConnected() == false)
			return;

		m_SendEvent.Init();
		m_SendEvent.m_Owner = shared_from_this(); // ADD_REF

		{
			WRITE_LOCK;

			int32 writeSize = 0;
			while (m_SendQueue.empty() == false)
			{
				SendBufferRef sendBuffer = m_SendQueue.front();

				writeSize += sendBuffer->WriteSize();

				m_SendQueue.pop();
				m_SendEvent.m_SendBuffers.push_back(sendBuffer);
			}
		}

		// Scatter-Gather
		Vector<WSABUF> wsaBufs;
		wsaBufs.reserve(m_SendEvent.m_SendBuffers.size());
		for (SendBufferRef sendBuffer : m_SendEvent.m_SendBuffers)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
			wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
			wsaBufs.push_back(wsaBuf);
		}

		DWORD numOfBytes = 0;
		if (SOCKET_ERROR == ::WSASend(m_Socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &m_SendEvent, nullptr))
		{
			int32 errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				HandleError(errorCode);
				m_SendEvent.m_Owner = nullptr; // RELEASE_REF
				m_SendEvent.m_SendBuffers.clear(); // RELEASE_REF
				m_SendRegistered.store(false);
			}
		}
	}

	void Session::ProcessConnect()
	{
		m_ConnectEvent.m_Owner = nullptr; // RELEASE_REF

		m_Connected.store(true);

		GetService()->AddSession(GetSessionRef());

		// reimplement on contents logic
		OnConnected();

		RegisterRecv();
	}

	void Session::ProcessDisconnect()
	{
		m_DisconnectEvent.m_Owner = nullptr; // RELEASE_REF

		OnDisconnected(); // reimplement on contents logic
		GetService()->ReleaseSession(GetSessionRef());
	}

	void Session::ProcessRecv(int32 numOfBytes)
	{
		m_RecvEvent.m_Owner = nullptr; // RELEASE_REF

		if (numOfBytes == 0)
		{
			Disconnect(L"Recv 0");
			return;
		}

		if (m_RecvBuffer.OnWrite(numOfBytes) == false)
		{
			Disconnect(L"OnWrite Overflow");
			return;
		}

		int32 dataSize = m_RecvBuffer.DataSize();
		int32 processLen = OnRecv(m_RecvBuffer.ReadPos(), dataSize); // reimplement on contents logic
		if (processLen < 0 || dataSize < processLen || m_RecvBuffer.OnRead(processLen) == false)
		{
			Disconnect(L"OnRead Overflow");
			return;
		}

		m_RecvBuffer.Clean();

		RegisterRecv();
	}

	void Session::ProcessSend(int32 numOfBytes)
	{
		m_SendEvent.m_Owner = nullptr; // RELEASE_REF
		m_SendEvent.m_SendBuffers.clear(); // RELEASE_REF

		if (numOfBytes == 0)
		{
			Disconnect(L"Send 0");
			return;
		}

		// reimplement on contents logic
		OnSend(numOfBytes);

		WRITE_LOCK;
		if (m_SendQueue.empty())
			m_SendRegistered.store(false);
		else
			RegisterSend();
	}

	void Session::HandleError(int32 errorCode)
	{
		switch (errorCode)
		{
		case WSAECONNRESET:
		case WSAECONNABORTED:
			Disconnect(L"HandleError");
			break;
		default:
			std::cout << "Handle Error : " << errorCode << std::endl;
			break;
		}
	}

	PacketSession::PacketSession()
	{

	}

	PacketSession::~PacketSession()
	{

	}

	// [size(2)][id(2)][data....][size(2)][id(2)][data....]
	int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
	{
		int32 processLen = 0;

		while (true)
		{
			int32 dataSize = len - processLen;
			if (dataSize < sizeof(PacketHeader))
				break;

			PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
			if (dataSize < header.size)
				break;

			OnRecvPacket(&buffer[processLen], header.size);

			processLen += header.size;
		}

		return processLen;
	}

}