#include "pch.hpp"

namespace Engine
{
	Session::Session() : m_RecvBuffer(BUFFER_SIZE)
	{

	}

	void Session::Send(SendBufferRef sendBuffer)
	{
		WRITE_LOCK;

		m_SendQueue.push(sendBuffer);

		if (m_SendRegistered.exchange(true) == true)
			return;

		m_SendEvent.SetOwner(shared_from_this());
		

		int32 writeSize = 0;
		while (m_SendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = m_SendQueue.front();

			writeSize += sendBuffer->WriteSize();

			m_SendQueue.pop();
			m_SendEvent.m_SendBuffers.push_back(sendBuffer);
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
		if (SOCKET_ERROR == ::WSASend(GetSocket(), wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &m_SendEvent, nullptr))
		{
			int32 lastError = ::WSAGetLastError();
			if (lastError != WSA_IO_PENDING)
			{
				HandleError(lastError);
				m_SendEvent.SetOwner(nullptr);
				m_SendEvent.m_SendBuffers.clear();
				m_SendRegistered.store(false);
			}
		}
	}

	void Session::Recv()
	{
		const SOCKET socket = GetSocket();
		
		WSABUF recvWSABuf;
		recvWSABuf.buf = reinterpret_cast<char*>(m_RecvBuffer.WritePos());
		recvWSABuf.len = m_RecvBuffer.FreeSize();
		
		DWORD recvLen = 0;
		DWORD flags = 0;
		
		IOCPEvent* iocpEvent = xnew<IOCPEvent>(EVENT_TYPE::RECV);
		if (::WSARecv(socket, &recvWSABuf, 1, &recvLen, &flags, iocpEvent, nullptr))
		{
			std::cout << "Session::Recv()" << std::endl;
			std::cout << "::WSARecv WSAGetLastError: " << ::WSAGetLastError() << std::endl;
		}
	}

	void Session::HandleError(int32 errorCode)
	{
		switch (errorCode)
		{
		case WSAECONNRESET:
		case WSAECONNABORTED:
			// Disconnect(L"HandleError");
			std::cout << "Handle Error : " << errorCode << std::endl;
			break;
		default:
			std::cout << "Handle Error : " << errorCode << std::endl;
			break;
		}
	}
}