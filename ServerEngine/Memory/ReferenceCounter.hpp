#pragma once

#include "pch.hpp"

#include "Core/ServerEnginePCH.hpp"

namespace Engine
{
	// WARN: cannot use with already exist classes, and can occur cycle dependency -> use weak_ptr
	class RefCounter
	{
	public:
		RefCounter() : m_RefCount(1) {};
		virtual ~RefCounter() {};

		const int32 GetRefCount() const { return m_RefCount; }

		const int32 AddRef() { return ++m_RefCount; }
		const int32 ReleaseRef()
		{
			const int32 refCount = --m_RefCount;
			if (refCount == 0) delete this;
			return refCount;
		}

	protected:
		std::atomic<int32> m_RefCount;
	};



	template<typename T>
	class TSharedPtr
	{
	public:
		TSharedPtr() {}
		TSharedPtr(T* ptr) { Set(ptr); }

		TSharedPtr(const TSharedPtr& rhs) { Set(rhs.m_ptr); }
		TSharedPtr(TSharedPtr&& rhs) { m_ptr = rhs.m_ptr; rhs.m_ptr = nullptr; }
		template<typename U>
		TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs.m_ptr)); }

		~TSharedPtr() { Release(); }

	public:
		TSharedPtr& operator=(const TSharedPtr& rhs)
		{
			if (m_ptr != rhs.m_ptr)
			{
				Release();
				Set(rhs.m_ptr);
			}

			return *this;
		}

		TSharedPtr& operator=(TSharedPtr&& rhs)
		{
			Release();
			m_ptr = rhs.m_ptr;
			rhs.m_ptr = nullptr;
			return *this;
		}

		bool		operator==(const TSharedPtr& rhs) const { return m_ptr == rhs.m_ptr; }
		bool		operator==(T* ptr) const { return m_ptr == ptr; }
		bool		operator!=(const TSharedPtr& rhs) const { return m_ptr != rhs.m_ptr; }
		bool		operator!=(T* ptr) const { return m_ptr != ptr; }
		bool		operator<(const TSharedPtr& rhs)  const { return m_ptr < rhs.m_ptr; }
		T*			operator*() { return m_ptr; }
		const T*	operator*() const { return m_ptr; }
					operator T* () const { return m_ptr; }
		T*			operator->() { return m_ptr; }
		const T*	operator->() const { return m_ptr; }

		bool IsNull() { return m_ptr == nullptr; }

	private:
		inline void Set(T * ptr)
		{
			m_ptr = ptr;
			if (ptr) ptr->AddRef();
		}

		inline void Release()
		{
			if (m_ptr != nullptr)
			{
				m_ptr->ReleaseRef();
				m_ptr = nullptr;
			}
		}

	private:
		T* m_ptr = nullptr;
	};
};