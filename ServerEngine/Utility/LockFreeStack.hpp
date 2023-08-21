#pragma once

namespace Engine
{
	template<typename T>
	class LockFreeStack
	{
		struct Node
		{
			Node(const T& value) : data(std::make_shared<T>(value)), next(nullptr) {}

			std::shared_ptr<T> data;
			std::shared_ptr<Node*> next;
		};

	public:
		/*
			1. create new Node(); 
			2. assign new Node->next = head;
			3. assign head = new Node();

			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		void Push(const T& value)
		{
			// 1. create new Node();
			shared_ptr<Node> node = make_shared<Node>(value);
			// 2. assign new Node->next = head;
			node->next = std::atomic_load(&m_Head);

			while (std::atomic_compare_exchange_weak(&m_Head, &node->next, node) == false) 
			{
			
			}
		}

		/*
			1. read head;
			2. read head->next;
			3. assign head = head->next;
			4. extract data to return;
			5. delete extracted Node;

			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		std::shared_ptr<T> TryPop()
		{
			shared_ptr<Node> oldHead = std::atomic_load(&m_Head);

			while (oldHead && std::atomic_compare_exchange_weak(&m_Head, &oldHead, oldHead->next) == false)
			{

			}

			if (oldHead == nullptr)
				return shared_ptr<T>();

			return oldHead->data;
		}
	private:
		/*
			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		std::shared_ptr<Node> m_Head;
	};
}