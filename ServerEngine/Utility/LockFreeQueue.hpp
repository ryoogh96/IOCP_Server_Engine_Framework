#pragma once

namespace Engine
{
	template<typename T>
	class LockFreeQueue
	{
		struct Node
		{
			std::shared_ptr<T> data;
			Node* next = nullptr;
		};

	public:
		LockFreeQueue() : m_Head(new Node), m_Tail(m_Head)
		{

		}

		LockFreeQueue(const LockFreeQueue&) = delete;
		LockFreeQueue& operator=(const LockFreeQueue&) = delete;

		void Push(const T& value)
		{
			std::shared_ptr<T> newData = std::make_shared<T>(value);

			Node* dummy = new Node();

			Node* oldTail = m_Tail;
			oldTail->data.swap(newData);
			oldTail->next = dummy;

			m_Tail = dummy;
		}

		std::shared_ptr<T> TryPop()
		{
			Node* oldHead = PopHead();
			if (oldHead == nullptr)
				return std::shared_ptr<T>();

			std::shared_ptr<T> res(oldHead->data);
			delete oldHead;
			return res;
		}

	private:
		Node* PopHead()
		{
			Node* oldHead = m_Head;
			if (oldHead == m_Tail)
				return nullptr;

			m_Head = oldHead->next;
			return oldHead;
		}

	private:
		// [data][data][]
		// [head][tail]
		Node* m_Head = nullptr;
		Node* m_Tail = nullptr;
	};
}