#include "pch.hpp"
#include "BufferReader.hpp"

namespace Engine
{
	BufferReader::BufferReader()
	{
	}

	BufferReader::BufferReader(BYTE* buffer, uint32 size, uint32 pos)
		: m_Buffer(buffer),m_Size(size), m_Pos(pos)
	{

	}

	BufferReader::~BufferReader()
	{

	}

	bool BufferReader::Peek(void* dest, uint32 len)
	{
		if (FreeSize() < len)
			return false;

		::memcpy(dest, &m_Buffer[m_Pos], len);
		return true;
	}

	bool BufferReader::Read(void* dest, uint32 len)
	{
		if (Peek(dest, len) == false)
			return false;

		m_Pos += len;
		return true;
	}
}