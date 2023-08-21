#pragma once

namespace Engine
{
	template<typename T>
	class LockQueue
	{
	public:
		LockQueue() {}

		LockQueue(const LockQueue&) = delete;
		LockQueue& operator=(const LockQueue&) = delete;

		void Push(T value)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_Queue.push(std::move(value));
			m_CondVar.notify_one();
		}

		bool TryPop(T& value)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			if (m_Queue.empty())
				return false;

			value = std::move(m_Queue.front());
			m_Queue.pop();
			return true;
		}

		void WaitPop(T& value)
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_CondVar.wait(lock, [this] { return m_Queue.empty() == false; });
			value = std::move(m_Queue.front());
			m_Queue.pop();
		}

	private:
		std::queue<T> m_Queue;
		std::mutex m_Mutex;
		std::condition_variable m_CondVar;
	};
}