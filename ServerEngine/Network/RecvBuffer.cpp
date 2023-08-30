#include "pch.hpp"
#include "RecvBuffer.hpp"

namespace Engine
{
	RecvBuffer::RecvBuffer(int32 bufferSize) : m_BufferSize(bufferSize)
	{
		m_Capacity = bufferSize * BUFFER_COUNT;
		m_Buffer.resize(m_Capacity);
	}

	RecvBuffer::~RecvBuffer()
	{
		std::cout << "~RecvBuffer()" << std::endl;
	}

	void RecvBuffer::Clean()
	{
		int32 dataSize = DataSize();
		if (dataSize == 0)
		{
			// if read,writePos are on the same position, reset both.
			m_ReadPos = m_WritePos = 0;
		}
		else
		{
			// if free size is less than 1 buffer, pull the datas to start position.
			if (FreeSize() < m_BufferSize)
			{
				::memcpy(&m_Buffer[0], &m_Buffer[m_ReadPos], dataSize);
				m_ReadPos = 0;
				m_WritePos = dataSize;
			}
		}
	}

	bool RecvBuffer::OnRead(int32 numOfBytes)
	{
		if (numOfBytes > DataSize())
			return false;

		m_ReadPos += numOfBytes;
		return true;
	}

	bool RecvBuffer::OnWrite(int32 numOfBytes)
	{
		if (numOfBytes > FreeSize())
			return false;

		m_WritePos += numOfBytes;
		return true;
	}
}