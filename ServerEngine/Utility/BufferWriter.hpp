#pragma once

namespace Engine
{
	class BufferWriter
	{
	public:
		BufferWriter();
		BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
		~BufferWriter();

		BYTE* Buffer() { return m_Buffer; }
		uint32			Size() { return m_Size; }
		uint32			WriteSize() { return m_Pos; }
		uint32			FreeSize() { return m_Size - m_Pos; }

		template<typename T>
		bool			Write(T* src) { return Write(src, sizeof(T)); }
		bool			Write(void* src, uint32 len);

		template<typename T>
		T* Reserve();

		template<typename T>
		BufferWriter& operator<<(const T& src);

		template<typename T>
		BufferWriter& operator<<(T&& src);

	private:
		BYTE*			m_Buffer = nullptr;
		uint32			m_Size = 0;
		uint32			m_Pos = 0;
	};

	template<typename T>
	T* BufferWriter::Reserve()
	{
		if (FreeSize() < sizeof(T))
			return nullptr;

		T* ret = reinterpret_cast<T*>(&m_Buffer[m_Pos]);
		m_Pos += sizeof(T);
		return ret;
	}

	template<typename T>
	BufferWriter& BufferWriter::operator<<(T&& src)
	{
		using DataType = std::remove_reference_t<T>;
		*reinterpret_cast<DataType*>(&m_Buffer[m_Pos]) = std::forward<DataType>(src);
		m_Pos += sizeof(T);
		return *this;
	}
}