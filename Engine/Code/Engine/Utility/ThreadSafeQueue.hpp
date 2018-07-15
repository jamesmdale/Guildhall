#pragma once
#include <queue>
#include "Engine\Utility\SpinLock.hpp"


template <typename T>
class ThreadSafeQueue
{
public:

	void enqueue(const T& value)
	{
		m_lock.Enter();

		//I AM THE ONLY PERSON HERE
		m_data.push(value);

		m_lock.Leave();
		//no longer true
	}

	bool dequeue(T* outValue)
	{
		m_lock.Enter();

		bool hasItem = !m_data.empty();
		if (hasItem)
		{
			*outValue = m_data.front();

			//cleanup
			/*delete(m_data.front());
			m_data.front() = nullptr;*/

			m_data.pop();
		}

		m_lock.Leave();

		//success
		return hasItem; 
	}

	size_t GetSize()
	{
		return m_data.size();
	}

public:
	std::queue<T> m_data;
	SpinLock m_lock;
};