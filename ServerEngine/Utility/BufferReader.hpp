#pragma once

namespace Engine
{
	class BufferReader
	{
	public:
		BufferReader();
		BufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
		~BufferReader();

		BYTE* Buffer() { return m_Buffer; }
		uint32			Size() { return m_Size; }
		uint32			ReadSize() { return m_Pos; }
		uint32			FreeSize() { return m_Size - m_Pos; }

		template<typename T>
		bool			Peek(T* dest) { return Peek(dest, sizeof(T)); }
		bool			Peek(void* dest, uint32 len);

		template<typename T>
		bool			Read(T* dest) { return Read(dest, sizeof(T)); }
		bool			Read(void* dest, uint32 len);

		template<typename T>
		BufferReader& operator>>(OUT T& dest);

	private:
		BYTE*			m_Buffer = nullptr;
		uint32			m_Size = 0;
		uint32			m_Pos = 0;
	};

	template<typename T>
	inline BufferReader& BufferReader::operator>>(OUT T& dest)
	{
		dest = *reinterpret_cast<T*>(&m_Buffer[m_Pos]);
		m_Pos += sizeof(T);
		return *this;
	}
}