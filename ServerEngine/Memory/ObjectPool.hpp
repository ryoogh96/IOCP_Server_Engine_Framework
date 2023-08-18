#pragma once

#include "Core/ServerEngineTypes.hpp"
#include "MemoryPool.hpp"

namespace Engine
{
	template<typename Type>
	class ObjectPool
	{
		template<typename... Args>
		static Type* Pop(Args&&... args)
		{
	#ifdef _STOMP
			MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_AllocSize));
			Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_AllocSize));
	#else
			Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_Pool.Pop(), s_AllocSize));
	#endif
			new(memory)Type(std::forward<Args>(args)...); // placement new, which designate memory location to use.
			return memory;
		}

		static void Push(Type* obj)
		{
			obj->~Type();
	#ifdef _STOMP
			StompAllocator::Release(MemoryHeader::DetachHeader(obj));
	#else
			s_Pool.Push(MemoryHeader::DetachHeader(obj));
	#endif
		}

		template<typename... Args>
		static std::shared_ptr<Type> MakeShared(Args&&... args)
		{
			std::shared_ptr<Type> ptr = { Pop(std::forward<Args>(args)...), Push };
			return ptr;
		}

	private:
		static int32		s_AllocSize;
		static MemoryPool	s_Pool;
	};

	template<typename Type>
	int32 ObjectPool<Type>::s_AllocSize = sizeof(Type) + sizeof(MemoryHeader);

	template<typename Type>
	MemoryPool ObjectPool<Type>::s_Pool{ s_AllocSize };
}