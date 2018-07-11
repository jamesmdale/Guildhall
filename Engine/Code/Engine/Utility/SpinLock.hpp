#pragma once
#include <mutex>

class SpinLock
{
public: 
	void Enter() { m_lock.lock(); }
	bool TryEnter(){ return m_lock.try_lock(); }
	void Leave(){ m_lock.unlock(); }

public:
	std::mutex m_lock;
};