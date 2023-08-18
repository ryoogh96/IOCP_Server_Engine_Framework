#include "pch.hpp"
#include "MemoryPool.hpp"

namespace Engine
{
	MemoryPool::MemoryPool(int32 allocSize) : m_AllocSize(allocSize)
	{
		::InitializeSListHead(&m_Header);
	}

	MemoryPool::~MemoryPool()
	{
		while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&m_Header)))
			::_aligned_free(memory);
	}

	void MemoryPool::Push(MemoryHeader* ptr)
	{
		ptr->m_AllocSize = 0;

		::InterlockedPushEntrySList(&m_Header, static_cast<PSLIST_ENTRY>(ptr));

		m_UseCount.fetch_sub(1);
		m_ReserveCount.fetch_add(1);
	}

	MemoryHeader* MemoryPool::Pop()
	{
		MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&m_Header));

		if (memory == nullptr)
		{
			memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(m_AllocSize, SLIST_ALIGNMENT));
		}
		else
		{
			ASSERT_CRASH(memory->m_AllocSize == 0);
			m_ReserveCount.fetch_sub(1);
		}

		m_UseCount.fetch_add(1);

		return memory;
	}
}