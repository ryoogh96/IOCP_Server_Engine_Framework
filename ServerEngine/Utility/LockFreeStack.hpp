#pragma once

namespace Engine
{
	template<typename T>
	class LockFreeStack
	{
		struct CountedNodePtr;

		struct Node
		{
			Node(const T& value) : data(std::make_shared<T>(value))
			{

			}

			std::shared_ptr<T> data;
			std::atomic<int32> internalCount = 0;
			CountedNodePtr next;
		};

		struct CountedNodePtr
		{
			int32 externalCount = 0;
			Node* ptr = nullptr;
		};

	public:
		/*
			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		void Push(const T& value)
		{
			CountedNodePtr node;
			node.ptr = new Node(value);
			node.externalCount = 1;

			node.ptr->next = m_Head;
			while (m_Head.compare_exchange_weak(node.ptr->next, node) == false) 
			{
			
			}
		}

		/*
			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		std::shared_ptr<T> TryPop()
		{
			CountedNodePtr oldHead = m_Head;
			while (true)
			{
				// the thread who has increased externalCount by 1 right now, acquire right to refer.
				IncreaseHeadCount(oldHead);
				// can safely access since externalCount >= 2.
				Node* ptr = oldHead.ptr;

				if (ptr == nullptr)
					return std::shared_ptr<T>();

				// the thread who has succeed to change head to ptr->next, acquire right to own.
				if (m_Head.compare_exchange_strong(oldHead, ptr->next))
				{
					std::shared_ptr<T> res;
					res.swap(ptr->data);

					// external: 1 -> 2(me+1) -> 4(me+1, other+2)
					// internal: 1 -> 0
					const int32 countIncrease = oldHead.externalCount - 2;

					if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
						delete ptr;

					return res;
				}
				else if (ptr->internalCount.fetch_sub(1) == 1)
				{
					// acquire right to refer, but fail to acquire right to own.
					delete ptr;
				}
			}
		}
	private:
		void IncreaseHeadCount(CountedNodePtr& oldCounter)
		{
			while (true)
			{
				CountedNodePtr newCounter = oldCounter;
				newCounter.externalCount++;

				if (m_Head.compare_exchange_strong(oldCounter, newCounter))
				{
					oldCounter.externalCount = newCounter.externalCount;
					break;
				}
			}
		}

	private:
		/*
			[ ][ ][ ][ ][ ][ ]
			[head]
		*/
		std::atomic<CountedNodePtr> m_Head;
	};
}