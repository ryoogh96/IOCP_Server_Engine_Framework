#pragma once

#include "pch.hpp"
#include "Memory.hpp"
#include "MemoryPool.hpp"

namespace Engine
{
	Memory::Memory()
	{
		int32 size = 0;
		int32 tableIndex = 0;

		for (size = 32; size <= 1024; size += 32)
		{
			MemoryPool* pool = xnew<MemoryPool>(size);
			m_Pools.push_back(pool);

			while (tableIndex <= size)
			{
				m_PoolTable[tableIndex] = pool;
				tableIndex++;
			}
		}

		for (; size <= 2048; size += 128)
		{
			MemoryPool* pool = xnew<MemoryPool>(size);
			m_Pools.push_back(pool);

			while (tableIndex <= size)
			{
				m_PoolTable[tableIndex] = pool;
				tableIndex++;
			}
		}

		for (; size <= 4096; size += 256)
		{
			MemoryPool* pool = xnew<MemoryPool>(size);
			m_Pools.push_back(pool);

			while (tableIndex <= size)
			{
				m_PoolTable[tableIndex] = pool;
				tableIndex++;
			}
		}
	}

	Memory::~Memory()
	{
		for (MemoryPool* pool : m_Pools)
			delete pool;

		m_Pools.clear();
	}

	void* Memory::Allocate(int32 size)
	{
		MemoryHeader* header = nullptr;
		const int32 allocSize = size + sizeof(MemoryHeader);

	#ifdef _STOMP
		header = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(allocSize));
	#else
		if (allocSize > MAX_ALLOC_SIZE)
		{
			header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
		}
		else
		{
			header = m_PoolTable[allocSize]->Pop();
		}
	#endif
		
		return MemoryHeader::AttachHeader(header, allocSize);
	}

	void Memory::Release(void* ptr)
	{
		MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

		const int32 allocSize = header->m_AllocSize;
		ASSERT_CRASH(allocSize > 0);

	#ifdef _STOMP
		StompAllocator::Release(header);
	#else
		if (allocSize > MAX_ALLOC_SIZE)
		{
			::_aligned_free(header);
		}
		else
		{
			m_PoolTable[allocSize]->Push(header);
		}
	#endif

	}
}