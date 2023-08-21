#pragma once

namespace Engine
{
	template<typename T>
	class LockFreeStack
	{
		struct Node
		{
			Node(const T& value) : data(value), next(nullptr) {}

			T data;
			Node* next;
		};

	public:
		/*
			1. create new Node(); 
			2. assign new Node->next = head;
			3. assign head = new Node();

			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		bool Push(T& value)
		{
			// 1. create new Node();
			Node* node = new Node(value);
			// 2. assign new Node->next = head;
			node->next = m_Head;

			while (m_Head.compare_exchange_weak(node->next, node) == false) 
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
		bool TryPop(T& value)
		{
			++_PopCount;

			// 1. read head;
			Node* oldHead = m_Head;


			// 2. read head->next;
			// 3. assign head = head->next;
			while (oldHead && m_Head.compare_exchange_weak(oldHead, oldHead->next) == false)
			{
				
			}


			if (oldHead == nullptr)
			{
				--m_PopCount;
				return false;
			}

			// 4. extract data to return;
			value = oldHead->data;
			// 5. delete extracted Node;
			TryDelete(oldHead);
			return true;
		}

		/*
			1. separate data
			2. check the popCount
			3. if I'm the only thread who's executing TryDelete logic, then delete nodes
		*/
		void TryDelete(Node* oldHead)
		{
			// 2. check the popCount
			if (m_PopCount == 1)
			{
				// also try to delete pending list together
				Node* node = m_PendingList.exchange(nullptr);

				if (--m_PopCount == 0)
				{
					// 3. if I'm the only thread who's executing TryDelete logic, then delete nodes
					// in the case of also another thread executing here, 
					// since the node already exchanged and become nullptr
					// there will be no problem.
					DeleteNodes(node);
				}
				else if (node)
				{
					// if another thread also entranced here, then put the node back to pending list
					ChainPendingNodeList(node);
				}

				delete oldHead;
			}
			else
			{
				// if another thread also entranced here, then put the node back to pending list
				ChainPendingNode(oldHead);
				--m_PopCount;
			}
		}

		void ChainPendingNodeList(Node* first, Node* last)
		{
			last->next = m_PendingList;

			while (m_PendingList.compare_exchange_weak(last->next, first) == false)
			{

			}
		}

		void ChainPendingNodeList(Node* node)
		{
			Node* last = node;
			while (last->next)
				last = last->next;

			ChainPendingNodeList(node, last);
		}

		void ChainPendingNode(Node* node)
		{
			ChainPendingNodeList(node, node);
		}

		static void DeleteNodes(Node* node)
		{
			while (node)
			{
				Node* node = node->next;
				delete node;
				node = next;
			}
		}

	private:
		/*
			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		std::atomic<Node*> m_Head;

		std::atomic<uint32> m_PopCount = 0; // the number of threads currently executing Pop
		std::atomic<Node*> m_PendingList; // the start(first) node of nodes to be deleted
	};
}