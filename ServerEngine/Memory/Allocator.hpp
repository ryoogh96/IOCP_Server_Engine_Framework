#pragma once

namespace Engine
{
	class BaseAllocator
	{
	public:
		static void*	Alloc(int32 size);
		static void		Release(void* ptr);
	};

	class StompAllocator
	{
		enum { PAGE_SIZE = 0x1000 };

	public:
		static void*	Alloc(int32 size);
		static void		Release(void* ptr);
	};

	class PoolAllocator
	{
	public:
		static void*	Alloc(int32 size);
		static void		Release(void* ptr);
	};
	
	template<typename T>
	class STLAllocator
	{
	public:
		using value_type = T;

		STLAllocator() {}

		template<typename Other>
		STLAllocator(const STLAllocator<Other>&) {}

		T* allocate(size_t count)
		{
			const int32 size = static_cast<int32>(count * sizeof(T));
			return static_cast<T*>(PoolAllocator::Alloc(size));
		}

		void deallocate(T* ptr, size_t count)
		{
			PoolAllocator::Release(ptr);
		}
	};
}