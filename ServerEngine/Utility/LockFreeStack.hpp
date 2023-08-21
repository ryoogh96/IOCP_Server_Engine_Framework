#pragma once

namespace Engine
{
	template<typename T>
	class LockFreeStack
	{
		struct Node
		{
			Node(const T& value) : data(value) {}

			T data;
			Node* next;
		};

	public:
		/*
			1. create new Node(); // thread-safe
			2. assign new Node->next = head; // thread-not-safe
			3. assign head = new Node(); // thread-not-safe

			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		bool Push(T& value)
		{
			// 1. create new Node(); // thread-safe
			Node* node = new Node(value);
			// 2. assign new Node->next = head; // thread-not-safe
			node->next = m_Head;

			/*
			if (m_Head == node->next)
			{
				m_Head = node;
				return true;
			}
			else
			{
				node->next = m_Head;
				return false;
			}
			*/

			while (m_Head.compare_exchange_weak(node->next, node) == false)
			{
				// node->next = m_Head;
			}


			// 3. what if another thread already assign different new Node();
			// m_Head = node;
		}

		/*
			1. read head; // thread-safe;
			2. read head->next; // thread-safe;
			3. assign head = head->next; // thread-not-safe;
			4. extract data to return; // thread-not-safe;
			5. delete extracted Node; // thread-not-safe;

			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		bool TryPop(T& value)
		{
			// 1. read head; // thread-safe;
			Node* oldHead = m_Head;

			if (m_Head == oldHead)
			{
				// 2. read head->next; // thread-safe;
				// 3. assign head = head->next; // thread-not-safe;
				m_Head = oldHead->next;
				return true;
			}
			else
			{
				oldHead = m_Head;
				return false;
			}

			while (oldHead && m_Head.compare_exchange_weak(oldHead, oldHead->next) == false)
			{
				// oldHead = m_Head;
			}

			// 4. extract data to return; // thread-not-safe;
			// exception not handled when oldHead is nullptr;
			value = oldHead->data;

			// 5. delete extracted Node; // thread-not-safe;
			// comment now for avoid when oldHead is nullptr
			// delete oldHead;


			return true;
		}

	private:
		/*
			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		std::atomic<Node*> m_Head;
	};
}