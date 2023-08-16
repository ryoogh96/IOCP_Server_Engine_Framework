#pragma once

#include "Core/ServerEnginePCH.hpp""

namespace Engine
{
	class RingBuffer
	{
	public:
		RingBuffer() {};
		~RingBuffer() {};
		const bool isEmpty() const { return m_head == m_tail; }
		// NOTE: 버퍼의 slot 하나를 쓰지 않고 남겨서 full 과 empty 상태를 구분한다.
		const bool isFull() const { return m_head + 1 == m_tail; }
		void push(char data[]);
		const char* pop() const;

	private:
		uint16 m_head = 0;
		uint16 m_tail = 0;
		char m_data[MAX_BUF_SIZE + 1] = { 0, };
	};
}