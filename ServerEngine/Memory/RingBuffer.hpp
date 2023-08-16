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
		// NOTE: ������ slot �ϳ��� ���� �ʰ� ���ܼ� full �� empty ���¸� �����Ѵ�.
		const bool isFull() const { return m_head + 1 == m_tail; }
		void push(char data[]);
		const char* pop() const;

	private:
		uint16 m_head = 0;
		uint16 m_tail = 0;
		char m_data[MAX_BUF_SIZE + 1] = { 0, };
	};
}