#pragma once

namespace Engine
{
	template<typename T>
	class LockStack
	{
	public:
		LockStack() {}

		LockStack(const LockStack&) = delete;
		LockStack& operator=(const LockStack&) = delete;

		void Push(T value)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_Stack.push(std::move(value));
			m_CondVar.notify_one();
		}

		bool TryPop(T& value)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			if (m_Stack.empty())
				return false;

			value = std::move(m_Stack.top());
			m_Stack.pop();
			return true;
		}

		void WaitPop(T& value)
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_CondVar.wait(lock, [this] { return m_Stack.empty() == false; });
			value = std::move(m_Stack.top());
			m_Stack.pop();
		}

	private:
		std::stack<T> m_Stack;
		std::mutex m_Mutex;
		std::condition_variable m_CondVar;
	};
}