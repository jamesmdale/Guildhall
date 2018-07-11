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
		m_data.push_back(value);

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
			m_data.pop_front();
		}

		m_lock.exit();

		//success
		return hasItem; 
	}

public:
	std::queue<T> m_data;
	SpinLock m_lock;
};