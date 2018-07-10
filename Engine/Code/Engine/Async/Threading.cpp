#include "Engine\Async\Threading.hpp"
#include <chrono>

ThreadHandle ThreadCreate(ThreadCallback callback, void* arguments)
{
	std::thread* threadObject = new std::thread(callback, arguments);

	s_threads.insert({(ThreadHandle)threadObject->get_id(), threadObject});

	return threadObject->get_id();
}

//  =========================================================================================
void ThreadJoin(ThreadHandle handle)
{
	std::thread* thread = GetThreadByHandle(handle);

	if(thread != nullptr)
		thread->join();

	thread = nullptr;
}

//  =========================================================================================
void ThreadDetach(ThreadHandle handle)
{
	std::thread* thread = GetThreadByHandle(handle);

	if (thread != nullptr)
		thread->detach();

	thread = nullptr;
}

//  =========================================================================================
void ThreadCreateAndDetach(ThreadCallback callback, void* userData)
{
	ThreadHandle handle = ThreadCreate(callback, userData);
	ThreadDetach(handle);
}

//  =========================================================================================
void ThreadSleep(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//  =========================================================================================
void ThreadYield()
{
	std::this_thread::yield();
}

//  =========================================================================================
std::thread* GetThreadByHandle(ThreadHandle handle)
{
	std::map<ThreadHandle, std::thread*>::iterator threadIterator;
	threadIterator = s_threads.find(handle);

	if (threadIterator != s_threads.end())
	{
		return threadIterator->second;
	}

	return nullptr;
}