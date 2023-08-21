#pragma once

namespace Engine
{
	template<typename T>
	class LockFreeQueue
	{
		struct Node;

		struct CountedNodePtr
		{
			int32 externalCount; // acquire right to refer
			Node* ptr = nullptr;
		};

		struct NodeCounter
		{
			uint32 internalCount : 30; // return acquire right to refer
			uint32 externalCountRemaining : 2; // acquire multiple right to refer Push & Pop
		};

		struct Node
		{
			Node()
			{
				NodeCounter newCount;
				newCount.internalCount = 0;
				newCount.externalCountRemaining = 2;
				count.store(newCount);

				next.ptr = nullptr;
				next.externalCount = 0;
			}

			void ReleaseRef()
			{
				NodeCounter oldCounter = count.load();

				while (true)
				{
					NodeCounter newCounter = oldCounter;
					newCounter.internalCount--;

					if (count.compare_exchange_strong(oldCounter, newCounter))
					{
						if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
							delete this;

						break;
					}
				}
			}

			std::atomic<T*> data;
			std::atomic<NodeCounter> count;
			CountedNodePtr next;
		};

	public:
		LockFreeQueue()
		{
			CountedNodePtr node;
			node.ptr = new Node;
			node.externalCount = 1;

			m_Head.store(node);
			m_Tail.store(node);
		}

		LockFreeQueue(const LockFreeQueue&) = delete;
		LockFreeQueue& operator=(const LockFreeQueue&) = delete;

		// [data][]
		// [head][tail]
		void Push(const T& value)
		{
			std::unique_ptr<T> newData = std::make_unique<T>(value);

			CountedNodePtr dummy;
			dummy.ptr = new Node;
			dummy.externalCount = 1;

			CountedNodePtr oldTail = m_Tail.load(); // ptr = nullptr

			while (true)
			{
				// the thread that increase externalCount by 1 right now, acquire right to refer
				IncreaseExternalCount(m_Tail, oldTail);

				// thet thread that exchange data first, acquire right to own
				T* oldData = nullptr;
				if (oldTail.ptr->data.compare_exchange_strong(oldData, newData.get()))
				{
					oldTail.ptr->next = dummy;
					oldTail = m_Tail.exchange(dummy);
					FreeExternalCount(oldTail);
					newData.release(); // give up the data of unique_ptr<T>
					break;
				}

				// the thread that fail to acquire right to own
				oldTail.ptr->ReleaseRef();
			}
		}

		std::shared_ptr<T> TryPop()
		{
			// [data][data][ ]
			// [head][tail]

			CountedNodePtr oldHead = m_Head.load();

			while (true)
			{
				// the thread that increase externalCount by 1 right now, acquire right to refer
				IncreaseExternalCount(m_Head, oldHead);

				Node* ptr = oldHead.ptr;
				if (ptr == m_Tail.load().ptr)
				{
					ptr->ReleaseRef();
					return std::shared_ptr<T>();
				}

				// acquire right to won (head = ptr->next)
				if (m_Head.compare_exchange_strong(oldHead, ptr->next))
				{
					T* res = ptr->data.load(); // there is bug when using exchange(nullptr);
					FreeExternalCount(oldHead);
					return std::shared_ptr<T>(res);
				}

				ptr->ReleaseRef();
			}
		}

	private:
		static void IncreaseExternalCount(std::atomic<CountedNodePtr>& counter, CountedNodePtr& oldCounter)
		{
			while (true)
			{
				CountedNodePtr newCounter = oldCounter;
				newCounter.externalCount++;

				if (counter.compare_exchange_strong(oldCounter, newCounter))
				{
					oldCounter.externalCount = newCounter.externalCount;
					break;
				}
			}
		}

		static void FreeExternalCount(CountedNodePtr& oldNodePtr)
		{
			Node* ptr = oldNodePtr.ptr;
			const int32 countIncrease = oldNodePtr.externalCount - 2;

			NodeCounter oldCounter = ptr->count.load();

			while (true)
			{
				NodeCounter newCounter = oldCounter;
				newCounter.externalCountRemaining--;
				newCounter.internalCount += countIncrease;

				if (ptr->count.compare_exchange_strong(oldCounter, newCounter))
				{
					if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
						delete ptr;

					break;
				}
			}
		}

	private:
		// [data][data][]
		// [head][tail]
		std::atomic<CountedNodePtr> m_Head;
		std::atomic<CountedNodePtr> m_Tail;
	};
}