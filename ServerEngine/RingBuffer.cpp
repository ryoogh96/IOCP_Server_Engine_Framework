#include "pch.hpp"

#include "IOCPManager.hpp"

namespace Engine
{
	void RingBuffer::push(char data[])
	{
		if (strlen(data) > MAX_BUF_SIZE)
		{
			std::cout << "data: " << data << " exceed MAX_BUF_SIZE to RingBuffer::push()" << std::endl;
			return;
		}


	}
}