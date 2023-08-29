#include "pch.hpp"
#include "SendBuffer.hpp"

namespace Engine
{
	SendBuffer::SendBuffer(int32 bufferSize)
	{
		m_Buffer.resize(bufferSize);
	}

	SendBuffer::~SendBuffer()
	{

	}

	void SendBuffer::CopyData(void* data, int32 len)
	{
		ASSERT_CRASH(Capacity() >= len);
		::memcpy(m_Buffer.data(), data, len);
		m_WriteSize = len;
	}
}