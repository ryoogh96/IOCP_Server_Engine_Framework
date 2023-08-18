#pragma once
#include "Allocator.hpp"

namespace Engine
{
	class Memory
	{
		enum
		{
			// 32 unit until ~1024, 128 unit until ~2048, 256 unit util ~4096
			POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
			MAX_ALLOC_SIZE = 4096
		};

	public:
		Memory();
		~Memory();

		void*	Allocate(int32 size);
		void	Release(void* ptr);
	
	private:
		std::vector<MemoryPool*> m_Pools;
		MemoryPool* m_PoolTable[MAX_ALLOC_SIZE + 1];
	};

	template<typename Type, typename... Args>
	Type* xnew(Args&&... args)
	{
		Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
		new(memory)Type(std::forward<Args>(args)...); // placement new, which designate memory location to use.
		return memory;
	}

	template<typename Type>
	void xdelete(Type* obj)
	{
		obj->~Type();
		PoolAllocator::Release(obj);
	}

	template<typename Type, typename... Args>
	std::shared_ptr<Type> MakeShared(Args&&... args)
	{
		return std::shared_ptr<Type> { xnew<Type>(std::forward<Args>(args)...), xdelete<Type> };
	}
}