#include "pch.hpp"

namespace Engine
{
	void RingBuffer::Push(const char data[])
	{
		if (strlen(data) > MAX_BUF_SIZE)
		{
			std::cout << "data: " << data << " exceed MAX_BUF_SIZE to RingBuffer::push()" << std::endl;
			return;
		}
		 
		memcpy(m_data[m_head], data, sizeof(data));

		m_head = (m_head + 1) % MAX_RING_SIZE;
		// NOTE: 버퍼가 다 찼을경우 tail 을 1 증가시켜서 가장 오래된 데이터 제거 효과 
		if (m_head == m_tail) m_tail = (m_tail + 1) % MAX_RING_SIZE;
	}

	const char* RingBuffer::Pop()
	{
		if (IsEmpty()) return "";

		char* result = new char[MAX_BUF_SIZE + 1];

		memcpy(result, m_data[m_tail], sizeof(m_data[m_tail]));

		m_tail = (m_tail + 1) % MAX_RING_SIZE;

		return result;
	}
}