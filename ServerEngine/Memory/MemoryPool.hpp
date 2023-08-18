#pragma once

#include "Core/ServerEngineTypes.hpp"

namespace Engine
{
	enum
	{
		// use 4 bytes to avoid ABA problem.
		// 0000 0000 0000 0000
		SLIST_ALIGNMENT = 16
	};

	DECLSPEC_ALIGN(SLIST_ALIGNMENT)
	struct MemoryHeader : public SLIST_ENTRY
	{
		// Structure: [MemoryHeader][Data]
		MemoryHeader(int32 size) : m_AllocSize() {}

		static void* AttachHeader(MemoryHeader* header, int32 size)
		{
			new(header)MemoryHeader(size); // placement new, which designate memory location to use.
			return reinterpret_cast<void*>(++header); // to directly use Data, move pointer after MemoryHeader and locate to start position of Data.
		}

		static MemoryHeader* DetachHeader(void* ptr)
		{
			MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
			return header;
		}

		int32 m_AllocSize;
	};

	DECLSPEC_ALIGN(SLIST_ALIGNMENT)
	class MemoryPool
	{
	public:
		MemoryPool(int32 allocSize);
		~MemoryPool();

		void			Push(MemoryHeader* ptr);
		MemoryHeader*	Pop();

	private:
		SLIST_HEADER	m_Header;
		int32			m_AllocSize = 0;
		std::atomic<int32>	m_UseCount = 0;
		std::atomic<int32>	m_ReserveCount = 0;
	};
}