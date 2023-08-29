#pragma once
namespace Engine
{
	class SendBuffer : std::enable_shared_from_this<SendBuffer>
	{
	public:
		SendBuffer(int32 bufferSize);
		~SendBuffer();

		BYTE* Buffer() { return m_Buffer.data(); }
		int32 WriteSize() { return m_WriteSize; }
		int32 Capacity() { return static_cast<int32>(m_Buffer.size()); }

		void CopyData(void* data, int32 len);

	private:
		Vector<BYTE>	m_Buffer;
		int32			m_WriteSize = 0;
	};
}