#include "pch.hpp"
#include "SendBuffer.hpp"

namespace Engine
{
	SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize)
		: m_Owner(owner), m_Buffer(buffer), m_AllocSize(allocSize)
	{

	}

	SendBuffer::~SendBuffer()
	{

	}

	void SendBuffer::Close(uint32 writeSize)
	{
		ASSERT_CRASH(m_AllocSize >= writeSize);
		m_WriteSize = writeSize;
		m_Owner->Close(writeSize);
	}

	SendBufferChunk::SendBufferChunk()
	{

	}

	SendBufferChunk::~SendBufferChunk()
	{

	}

	void SendBufferChunk::Reset()
	{
		m_Open = false;
		m_UsedSize = 0;
	}

	SendBufferRef SendBufferChunk::Open(uint32 allocSize)
	{
		ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
		ASSERT_CRASH(m_Open == false);

		if (allocSize > FreeSize())
			return nullptr;

		m_Open = true;
		return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
	}

	void SendBufferChunk::Close(uint32 writeSize)
	{
		ASSERT_CRASH(m_Open == true);
		m_Open = false;
		m_UsedSize += writeSize;
	}

	SendBufferRef SendBufferManager::Open(uint32 size)
	{
		if (LSendBufferChunk == nullptr)
		{
			LSendBufferChunk = Pop(); // WRITE_LOCK
			LSendBufferChunk->Reset();
		}

		ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

		if (LSendBufferChunk->FreeSize() < size)
		{
			LSendBufferChunk = Pop(); // WRITE_LOCK
			LSendBufferChunk->Reset();
		}

		std::cout << "SendBufferManager::Open(uint32 size)" << std::endl;
		std::cout << "FREE: " << LSendBufferChunk->FreeSize() << std::endl;

		return LSendBufferChunk->Open(size);
	}

	SendBufferChunkRef SendBufferManager::Pop()
	{
		std::cout << "SendBufferChunkRef SendBufferManager::Pop()" << std::endl;

		{
			WRITE_LOCK;
			if (m_SendBufferChunks.empty() == false)
			{
				SendBufferChunkRef sendBufferChunk = m_SendBufferChunks.back();
				m_SendBufferChunks.pop_back();
				return sendBufferChunk;
			}
		}

		return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
	}

	void SendBufferManager::Push(SendBufferChunkRef buffer)
	{
		WRITE_LOCK;
		m_SendBufferChunks.push_back(buffer);
	}

	void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
	{
		std::cout << "void SendBufferManager::PushGlobal" << std::endl;
		GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
	}
}