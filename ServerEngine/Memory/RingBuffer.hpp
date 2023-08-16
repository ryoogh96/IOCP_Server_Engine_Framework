#pragma once

#include "Core/ServerEnginePCH.hpp"

#define MAX_RING_SIZE 3

namespace Engine
{
	class RingBuffer
	{
	public:
		RingBuffer() {};
		~RingBuffer() {};

		const bool IsEmpty() const { return m_head == m_tail; }
		void Push(const char data[]);
		const char* Pop();

	private:
		uint16 m_head = 0;
		uint16 m_tail = 0;
		char m_data[MAX_RING_SIZE + 1][MAX_BUF_SIZE + 1] = { 0, };
	};
}